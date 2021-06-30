#include <stdio.h>

#if !defined(USE_STB_VORBIS) && !defined(USE_OGGVORBIS)
#define USE_STB_VORBIS
#endif

#if defined(USE_OGGVORBIS)
#   ifndef OV_EXCLUDE_STATIC_CALLBACKS
#	    define OV_EXCLUDE_STATIC_CALLBACKS
#   endif
#	include <vorbis/vorbisfile.h>
#endif

#ifdef __APPLE__
#define MA_NO_RUNTIME_LINKING
#endif

#define MA_LOG_LEVEL MA_LOG_LEVEL_VERBOSE

#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_OPUS
#define MA_NO_CUSTOM
#define MA_NO_ENCODING
#define MA_PREFER_AVX2

#include "extras/speex_resampler/ma_speex_resampler.h"

#if defined(USE_OGGVORBIS)

#define MA_NO_VORBIS

#include "miniaudio.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "miniaudio_engine.h"

#elif defined(USE_STB_VORBIS)

#define STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "miniaudio_engine.h"

#undef STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"

#else
#error "Select an ogg/vorbis decoder implementation!"
#endif

#define MINIAUDIO_SPEEX_RESAMPLER_IMPLEMENTATION
#include "extras/speex_resampler/ma_speex_resampler.h"

#include "extras/miniaudio_libvorbis.h"

static ma_result ma_decoding_backend_init__libvorbis(void *pUserData, ma_read_proc onRead, ma_seek_proc onSeek, ma_tell_proc onTell, void *pReadSeekTellUserData, const ma_decoding_backend_config *pConfig, const ma_allocation_callbacks *pAllocationCallbacks, ma_data_source **ppBackend)
{
	ma_result result;
	ma_libvorbis *pVorbis;

	(void)pUserData;

	pVorbis = (ma_libvorbis *)ma_malloc(sizeof(*pVorbis), pAllocationCallbacks);
	if (pVorbis == NULL) {
		return MA_OUT_OF_MEMORY;
	}

	result = ma_libvorbis_init(onRead, onSeek, onTell, pReadSeekTellUserData, pConfig, pAllocationCallbacks, pVorbis);
	if (result != MA_SUCCESS) {
		return result;
	}

	*ppBackend = pVorbis;

	return MA_SUCCESS;
}

static ma_result ma_decoding_backend_init_file__libvorbis(void *pUserData, const char *pFilePath, const ma_decoding_backend_config *pConfig, const ma_allocation_callbacks *pAllocationCallbacks, ma_data_source **ppBackend)
{
	ma_result result;
	ma_libvorbis *pVorbis;

	(void)pUserData;

	pVorbis = (ma_libvorbis *)ma_malloc(sizeof(*pVorbis), pAllocationCallbacks);
	if (pVorbis == NULL) {
		return MA_OUT_OF_MEMORY;
	}

	result = ma_libvorbis_init_file(pFilePath, pConfig, pAllocationCallbacks, pVorbis);
	if (result != MA_SUCCESS) {
		return result;
	}

	*ppBackend = pVorbis;

	return MA_SUCCESS;
}

static void ma_decoding_backend_uninit__libvorbis(void *pUserData, ma_data_source *pBackend, const ma_allocation_callbacks *pAllocationCallbacks)
{
	ma_libvorbis *pVorbis = (ma_libvorbis *)pBackend;

	(void)pUserData;

	ma_libvorbis_uninit(pVorbis, pAllocationCallbacks);
	ma_free(pVorbis, pAllocationCallbacks);
}

static ma_result ma_decoding_backend_get_channel_map__libvorbis(void *pUserData, ma_data_source *pBackend, ma_channel *pChannelMap, size_t channelMapCap)
{
	ma_libvorbis *pVorbis = (ma_libvorbis *)pBackend;

	(void)pUserData;

	return ma_libvorbis_get_data_format(pVorbis, NULL, NULL, NULL, pChannelMap, channelMapCap);
}

ma_decoding_backend_vtable g_ma_decoding_backend_vtable_libvorbis = {
	ma_decoding_backend_init__libvorbis,
	ma_decoding_backend_init_file__libvorbis,
	NULL, /* onInitFileW() */
	NULL, /* onInitMemory() */
	ma_decoding_backend_uninit__libvorbis,
	ma_decoding_backend_get_channel_map__libvorbis
};
