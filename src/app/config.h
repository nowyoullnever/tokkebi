#pragma once

#define PLUG_NAME "tokkebi"
#define PLUG_MFR "nowyoullnever"
#define PLUG_VERSION_HEX 0x00000100
#define PLUG_VERSION_STR "0.1.0"
#define PLUG_UNIQUE_ID 'Tkb1'
#define PLUG_MFR_ID 'NYN1'
#define PLUG_URL_STR "https://github.com/nowyoullnever/tokkebi"
#define PLUG_EMAIL_STR ""
#define PLUG_COPYRIGHT_STR "Copyright 2026 nowyoullnever"
#define PLUG_CLASS_NAME Tokkebi

#define BUNDLE_NAME "tokkebi"
#define BUNDLE_MFR "nowyoullnever"
#define BUNDLE_DOMAIN "com"

#define SHARED_RESOURCES_SUBPATH "tokkebi"
#if defined(APP_API)
  // The Standalone bootstrap is output-only: do not open a live input device.
  #define PLUG_CHANNEL_IO "0-2"
#else
  // Future plugin targets retain the transparent 2-in/2-out utility topology.
  #define PLUG_CHANNEL_IO "2-2"
#endif
#define PLUG_LATENCY 0
#define PLUG_TYPE 0
#define PLUG_DOES_MIDI_IN 0
#define PLUG_DOES_MIDI_OUT 0
#define PLUG_DOES_MPE 0
#define PLUG_DOES_STATE_CHUNKS 0
#define PLUG_HAS_UI 1
#define PLUG_WIDTH 1160
#define PLUG_HEIGHT 760
#define PLUG_FPS 60
#define PLUG_SHARED_RESOURCES 0
#define PLUG_HOST_RESIZE 0

#define AUV2_ENTRY tokkebi_Entry
#define AUV2_ENTRY_STR "tokkebi_Entry"
#define AUV2_FACTORY tokkebi_Factory
#define AUV2_VIEW_CLASS tokkebi_View
#define AUV2_VIEW_CLASS_STR "tokkebi_View"

#define VST3_SUBCATEGORY "Fx|Tools"

#define APP_NUM_CHANNELS 2
#define APP_N_VECTOR_WAIT 0
#define APP_MULT 1
#define APP_COPY_AUV3 0
#define APP_SIGNAL_VECTOR_SIZE 64
