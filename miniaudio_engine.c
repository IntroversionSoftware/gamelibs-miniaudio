#ifdef __APPLE__
#define MA_NO_RUNTIME_LINKING
#endif

#define MA_LOG_LEVEL MA_LOG_LEVEL_VERBOSE

#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_CUSTOM
#define MA_NO_ENCODING
#define MA_PREFER_AVX2

#include "extras/speex_resampler/ma_speex_resampler.h"

#define STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "miniaudio_engine.h"

#undef STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"

#define MINIAUDIO_SPEEX_RESAMPLER_IMPLEMENTATION
#include "extras/speex_resampler/ma_speex_resampler.h"
