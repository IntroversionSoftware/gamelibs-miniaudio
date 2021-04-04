#ifdef _DEBUG
#define MA_LOG_LEVEL MA_LOG_LEVEL_VERBOSE
#endif

#ifdef __APPLE__
#define MA_NO_RUNTIME_LINKING
#endif

#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_CUSTOM
#define MA_NO_ENCODING
#define MA_PREFER_AVX2
#define MA_MAX_CHANNELS 8

#define USE_OGGVORBIS

#include "extras/speex_resampler/ma_speex_resampler.h"

#if defined(USE_OGGVORBIS)

#include "miniaudio.h"
#include "miniaudio_vorbis.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "miniaudio_engine.h"

#include "miniaudio_vorbis.h"

#else

#define STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "miniaudio_engine.h"

#undef STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"

#endif

#define MINIAUDIO_SPEEX_RESAMPLER_IMPLEMENTATION
#include "extras/speex_resampler/ma_speex_resampler.h"
