#define STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"

#define MA_IMPLEMENTATION
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_DEBUG_OUTPUT
#define MA_LOG_LEVEL MA_LOG_LEVEL_VERBOSE
#include "miniaudio.h"
#include "miniaudio_engine.h"

#undef STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"
