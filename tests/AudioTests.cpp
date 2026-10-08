#include "audio/AudioDecoder.h"
#include "audio/AudioDocument.h"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

namespace { int failures = 0;
#define CHECK(expression) do { if (!(expression)) { std::cerr << "FAILED: " #expression "\\n"; ++failures; } } while (false)
void U16(std::vector<unsigned char>& b, uint16_t v) { b.push_back(static_cast<unsigned char>(v)); b.push_back(static_cast<unsigned char>(v >> 8)); }
void U32(std::vector<unsigned char>& b, uint32_t v) { for (int i = 0; i < 4; ++i) b.push_back(static_cast<unsigned char>(v >> (i * 8))); }
void Tag(std::vector<unsigned char>& b, const char* t) { b.insert(b.end(), t, t + 4); }
void PatchU32(std::vector<unsigned char>& b, size_t p, uint32_t v) { for (int i = 0; i < 4; ++i) b[p + i] = static_cast<unsigned char>(v >> (i * 8)); }
std::vector<unsigned char> Wav(uint16_t f = 1, uint16_t c = 1, uint32_t r = 48000, uint16_t bits = 16, uint16_t align = 0, const std::vector<unsigned char>& data = {}) { const uint16_t a = align == 0 ? static_cast<uint16_t>(c * (bits / 8)) : align; std::vector<unsigned char> b; Tag(b,"RIFF"); U32(b,0); Tag(b,"WAVE"); Tag(b,"fmt "); U32(b,16); U16(b,f); U16(b,c); U32(b,r); U32(b,r*a); U16(b,a); U16(b,bits); Tag(b,"data"); U32(b,static_cast<uint32_t>(data.size())); b.insert(b.end(),data.begin(),data.end()); if(data.size()&1U)b.push_back(0); PatchU32(b,4,static_cast<uint32_t>(b.size()-8)); return b; }
void Write(const std::filesystem::path& p, const std::vector<unsigned char>& b) { std::ofstream o(p,std::ios::binary); o.write(reinterpret_cast<const char*>(b.data()),static_cast<std::streamsize>(b.size())); }
}
int main() { using namespace tokkebi::audio; const auto root = std::filesystem::temp_directory_path()/"tokkebi-p02-8-audio-tests"; std::error_code ec; std::filesystem::remove_all(root,ec); std::filesystem::create_directories(root,ec); CHECK(!ec);
  const std::vector<unsigned char> mono16 {0x00,0x80,0x00,0x00,0x00,0x40,0xff,0x7f}; const auto wav = root/"fixture.wav"; Write(wav,Wav(1,1,48000,16,0,mono16)); auto decoded=DecodeLocalAudio(wav); CHECK(decoded.Success()); CHECK(decoded.info.container==AudioContainer::Wav); CHECK(decoded.info.channels==1); CHECK(decoded.info.sampleRate==48000); CHECK(decoded.info.frames==4); CHECK(decoded.audio.interleaved.size()==4); CHECK(std::abs(decoded.audio.At(0,0)+1.f)<.0001f); CHECK(std::abs(decoded.audio.At(2,0)-.5f)<.0001f);
  const auto extension=root/"fixture.not-audio"; Write(extension,Wav(1,1,48000,16,0,mono16)); CHECK(DecodeLocalAudio(extension).Success()); const auto unicode=root/std::filesystem::u8path(u8"한글-오디오.wav"); Write(unicode,Wav(1,1,48000,16,0,mono16)); CHECK(DecodeLocalAudio(unicode).Success());
  const std::vector<unsigned char> stereo24 {0x00,0x00,0x80,0x00,0x00,0x00,0xff,0xff,0x7f,0x00,0x00,0x40}; const auto p24=root/"pcm24.wav"; Write(p24,Wav(1,2,44100,24,0,stereo24)); auto d24=DecodeLocalAudio(p24); CHECK(d24.Success()); CHECK(d24.info.frames==2); CHECK(d24.info.channels==2); CHECK(std::abs(d24.audio.At(0,0)+1.f)<.0001f);
  float quarter=.25f; std::vector<unsigned char> floatData(sizeof(float)); std::memcpy(floatData.data(),&quarter,sizeof(float)); const auto fp=root/"float.wav"; Write(fp,Wav(3,1,48000,32,0,floatData)); auto fl=DecodeLocalAudio(fp); CHECK(fl.Success()); CHECK(fl.info.sourceFormat==SampleFormat::Float32); CHECK(std::abs(fl.audio.At(0,0)-.25f)<.0001f);
  CHECK(DecodeLocalAudio(root/"missing.wav").error==DecodeErrorCode::FileNotFound); CHECK(DecodeLocalAudio(root).error==DecodeErrorCode::NotRegularFile); Write(root/"empty.wav",{}); CHECK(DecodeLocalAudio(root/"empty.wav").error==DecodeErrorCode::TruncatedFile); Write(root/"truncated.wav",{'R','I','F','F',0,0,0,0,'W','A','V','E','f','m'}); CHECK(DecodeLocalAudio(root/"truncated.wav").error==DecodeErrorCode::TruncatedFile);
  auto noData=Wav(); noData.resize(36); PatchU32(noData,4,static_cast<uint32_t>(noData.size()-8)); Write(root/"no-data.wav",noData); CHECK(DecodeLocalAudio(root/"no-data.wav").error==DecodeErrorCode::CorruptFile); Write(root/"bad-align.wav",Wav(1,1,48000,16,1,mono16)); CHECK(DecodeLocalAudio(root/"bad-align.wav").error==DecodeErrorCode::InvalidMetadata); Write(root/"unsupported.wav",Wav(1,1,48000,8,0,{0})); CHECK(DecodeLocalAudio(root/"unsupported.wav").error==DecodeErrorCode::UnsupportedFormat);
  Write(root/"aiff.bin",{'F','O','R','M',0,0,0,0,'A','I','F','F'}); auto ai=DecodeLocalAudio(root/"aiff.bin"); CHECK(ai.error==DecodeErrorCode::UnsupportedFormat); CHECK(ai.info.container==AudioContainer::Aiff); Write(root/"flac.bin",{'f','L','a','C',0,0,0,0,0,0,0,0}); auto fc=DecodeLocalAudio(root/"flac.bin"); CHECK(fc.error==DecodeErrorCode::UnsupportedFormat); CHECK(fc.info.container==AudioContainer::Flac);
  CHECK(FramesToSeconds(48000,48000)==1.0); CHECK(SecondsToFrames(1.5,48000)==72000); CHECK(FramesToMilliseconds(24000,48000)==500.0); CHECK(MillisecondsToFrames(250.0,48000)==12000); CHECK(SecondsToFrames(-1,48000)==0); CHECK(SecondsToFrames(std::numeric_limits<double>::max(),48000)==0); CHECK(MillisecondsToFrames(std::numeric_limits<double>::infinity(),48000)==0);
  AudioDocument doc; CHECK(doc.State()==AudioDocumentState::Empty); doc.BeginLoad(); CHECK(doc.State()==AudioDocumentState::Loading); doc.LoadLocalFile(wav); CHECK(doc.State()==AudioDocumentState::Ready); CHECK(doc.Pcm().frames==4); doc.LoadLocalFile(root/"missing.wav"); CHECK(doc.State()==AudioDocumentState::Failed); CHECK(doc.Error()==DecodeErrorCode::FileNotFound); doc.Clear(); CHECK(doc.State()==AudioDocumentState::Empty); std::filesystem::remove_all(root,ec); return failures==0?0:1; }
