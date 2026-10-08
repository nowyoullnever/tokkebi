#include "AudioDecoder.h"

#include <cerrno>
#include <cmath>
#include <cstring>
#include <fstream>
#include <limits>
#include <new>
#include <system_error>

namespace tokkebi::audio {
namespace {
constexpr uint64_t kMaximumInputBytes = 1024ULL * 1024ULL * 1024ULL;
uint16_t U16(const unsigned char* p) { return static_cast<uint16_t>(p[0] | (static_cast<uint16_t>(p[1]) << 8)); }
uint32_t U32(const unsigned char* p) { return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) | (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24); }
bool Tag(const unsigned char* p, const char* tag) { return std::memcmp(p, tag, 4) == 0; }
void Fail(AudioLoadResult& r, DecodeErrorCode c, const char* text) { r.error = c; r.diagnostic = text; }
DecodeErrorCode FsError(const std::error_code& e) { return e == std::errc::permission_denied ? DecodeErrorCode::PermissionDenied : DecodeErrorCode::IoError; }
}

bool CalculateDecodedAllocation(uint64_t frames, uint32_t channels, DecodedAllocation& allocation) {
  allocation = {};
  if (channels == 0 || frames > std::numeric_limits<uint64_t>::max() / channels) return false;
  allocation.sampleCount = frames * channels;
  if (allocation.sampleCount > std::numeric_limits<uint64_t>::max() / sizeof(float)) return false;
  allocation.bytes = allocation.sampleCount * sizeof(float);
  return allocation.sampleCount <= std::numeric_limits<size_t>::max() && allocation.bytes <= kMaximumDecodedBytes;
}

double FramesToSeconds(uint64_t frames, uint32_t rate) { return rate == 0 ? 0.0 : static_cast<double>(frames) / rate; }
uint64_t SecondsToFrames(double seconds, uint32_t rate) {
  if (!std::isfinite(seconds) || seconds < 0.0 || rate == 0) return 0;
  const double exact = seconds * static_cast<double>(rate);
  const double safeLimit = static_cast<double>(std::numeric_limits<long long>::max());
  if (!std::isfinite(exact) || exact >= safeLimit) return 0;
  return static_cast<uint64_t>(std::floor(exact + .5));
}
double FramesToMilliseconds(uint64_t frames, uint32_t rate) { return FramesToSeconds(frames, rate) * 1000.0; }
uint64_t MillisecondsToFrames(double ms, uint32_t rate) { return !std::isfinite(ms) || ms < 0.0 ? 0 : SecondsToFrames(ms / 1000.0, rate); }

AudioLoadResult DecodeLocalAudio(const std::filesystem::path& path) {
  AudioLoadResult out; out.info.path = path; std::error_code ec;
  if (!std::filesystem::exists(path, ec)) { Fail(out, ec ? FsError(ec) : DecodeErrorCode::FileNotFound, "The selected local file does not exist or cannot be inspected."); return out; }
  if (!std::filesystem::is_regular_file(path, ec) || ec) { Fail(out, ec ? FsError(ec) : DecodeErrorCode::NotRegularFile, "The selected path is not a regular file."); return out; }
  out.info.fileSize = std::filesystem::file_size(path, ec);
  if (ec) { Fail(out, FsError(ec), "Could not determine the selected file size."); return out; }
  if (out.info.fileSize > kMaximumInputBytes) { Fail(out, DecodeErrorCode::TooLarge, "The selected file exceeds the 1 GiB input safety limit."); return out; }
  std::ifstream in(path, std::ios::binary);
  if (!in) { Fail(out, errno == EACCES ? DecodeErrorCode::PermissionDenied : DecodeErrorCode::IoError, "Could not open the selected local file."); return out; }
  std::vector<unsigned char> b((std::istreambuf_iterator<char>(in)), {});
  if (in.bad() || b.size() != out.info.fileSize) { Fail(out, DecodeErrorCode::IoError, "Could not read the selected local file completely."); return out; }
  if (b.size() < 12) { Fail(out, DecodeErrorCode::TruncatedFile, "The selected file is shorter than an audio container header."); return out; }
  if (Tag(b.data(), "FORM") && (Tag(b.data()+8,"AIFF") || Tag(b.data()+8,"AIFC"))) { out.info.container=AudioContainer::Aiff; Fail(out,DecodeErrorCode::UnsupportedFormat,"AIFF is recognized but unsupported in P02.8."); return out; }
  if (Tag(b.data(), "fLaC")) { out.info.container=AudioContainer::Flac; Fail(out,DecodeErrorCode::UnsupportedFormat,"FLAC is recognized but unsupported in P02.8."); return out; }
  if (!Tag(b.data(),"RIFF") || !Tag(b.data()+8,"WAVE")) { Fail(out,DecodeErrorCode::UnsupportedFormat,"The selected file is not a supported WAV container."); return out; }
  out.info.container=AudioContainer::Wav;
  const uint64_t declared=U32(b.data()+4); if (declared < 4 || declared > std::numeric_limits<uint64_t>::max()-8) { Fail(out,DecodeErrorCode::InvalidMetadata,"The RIFF declared size is invalid."); return out; }
  const uint64_t riffEnd=8+declared; if (riffEnd > b.size()) { Fail(out,DecodeErrorCode::TruncatedFile,"The RIFF declared size exceeds the physical file."); return out; }
  size_t pos=12, dataOffset=0; uint32_t dataSize=0, rate=0; uint16_t fmt=0,ch=0,align=0,bits=0; bool gotFmt=false,gotData=false;
  while (pos < riffEnd) {
    if (riffEnd-pos < 8) { Fail(out,DecodeErrorCode::TruncatedFile,"A WAV chunk header crosses the RIFF boundary."); return out; }
    const uint32_t n=U32(b.data()+pos+4); const uint64_t payload=static_cast<uint64_t>(pos)+8;
    if (n > riffEnd-payload) { Fail(out,DecodeErrorCode::TruncatedFile,"A WAV chunk payload crosses the RIFF boundary."); return out; }
    if (Tag(b.data()+pos,"fmt ") && !gotFmt) { if(n<16){Fail(out,DecodeErrorCode::InvalidMetadata,"The WAV format chunk is too short.");return out;} fmt=U16(b.data()+payload);ch=U16(b.data()+payload+2);rate=U32(b.data()+payload+4);align=U16(b.data()+payload+12);bits=U16(b.data()+payload+14);gotFmt=true; }
    else if (Tag(b.data()+pos,"data") && !gotData) { dataOffset=static_cast<size_t>(payload);dataSize=n;gotData=true; }
    const uint64_t next=payload+n+(n&1U); if(next>riffEnd){Fail(out,DecodeErrorCode::TruncatedFile,"A WAV chunk padding byte crosses the RIFF boundary.");return out;} pos=static_cast<size_t>(next);
  }
  if(!gotFmt||!gotData){Fail(out,DecodeErrorCode::CorruptFile,"The RIFF container is missing a required format or data chunk.");return out;}
  if(ch==0||ch>32||rate==0){Fail(out,DecodeErrorCode::InvalidMetadata,"The WAV channel count or sample rate is invalid.");return out;}
  if((fmt!=1&&fmt!=3)||(bits!=16&&bits!=24&&bits!=32)||(fmt==3&&bits!=32)){Fail(out,DecodeErrorCode::UnsupportedFormat,"Only PCM 16/24/32-bit and IEEE float32 WAV are supported.");return out;}
  const uint64_t per=bits/8, expected=static_cast<uint64_t>(ch)*per;
  if(align!=expected||dataSize%expected!=0){Fail(out,DecodeErrorCode::InvalidMetadata,"The WAV block alignment or data length is invalid. Byte-rate is intentionally not validated for compatibility.");return out;}
  const uint64_t frames=dataSize/expected; DecodedAllocation allocation;
  if(!CalculateDecodedAllocation(frames,ch,allocation)){Fail(out,DecodeErrorCode::TooLarge,"Decoded PCM would exceed the 512 MiB safety budget or overflow.");return out;}
  out.info.codec=fmt==1?AudioCodec::Pcm:AudioCodec::IeeeFloat; out.info.sourceFormat=bits==16?SampleFormat::Int16:bits==24?SampleFormat::Int24:fmt==3?SampleFormat::Float32:SampleFormat::Int32; out.info.channels=ch;out.info.sampleRate=rate;out.info.frames=frames;out.info.durationSeconds=FramesToSeconds(frames,rate);
  out.audio={ch,rate,frames,{}};
  try { out.audio.interleaved.resize(static_cast<size_t>(allocation.sampleCount)); } catch (const std::bad_alloc&) { Fail(out,DecodeErrorCode::TooLarge,"Decoded PCM allocation failed."); return out; } catch (const std::length_error&) { Fail(out,DecodeErrorCode::TooLarge,"Decoded PCM allocation exceeds vector capacity."); return out; }
  for(uint64_t i=0;i<allocation.sampleCount;++i){const auto* p=b.data()+dataOffset+i*per;float value=0.f;if(fmt==3){std::memcpy(&value,p,4);if(!std::isfinite(value)){Fail(out,DecodeErrorCode::InvalidMetadata,"The WAV float payload contains a non-finite sample.");return out;}}else if(bits==16)value=static_cast<float>(static_cast<int16_t>(U16(p)))/32768.f;else if(bits==24){int32_t v=static_cast<int32_t>(p[0])|(static_cast<int32_t>(p[1])<<8)|(static_cast<int32_t>(p[2])<<16);if(v&0x800000)v|=~0xffffff;value=static_cast<float>(v)/8388608.f;}else value=static_cast<float>(static_cast<int32_t>(U32(p)))/2147483648.f;out.audio.interleaved[static_cast<size_t>(i)]=value;}
  return out;
}
}
