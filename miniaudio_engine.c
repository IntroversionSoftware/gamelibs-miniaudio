#ifdef _DEBUG
#define MA_DEBUG_OUTPUT
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

#include "miniaudio.h"
#include "miniaudio_vorbis.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "miniaudio_engine.h"

#include "miniaudio_vorbis.h"
