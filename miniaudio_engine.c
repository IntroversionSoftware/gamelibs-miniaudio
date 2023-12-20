#include <stdio.h>

#if !defined(USE_STB_VORBIS) && !defined(USE_OGGVORBIS)
#define USE_STB_VORBIS
#endif

#ifdef __APPLE__
#define MA_NO_RUNTIME_LINKING
#endif

#define MA_DEBUG_OUTPUT
#define MA_MAX_CHANNELS 16

#define MA_NO_WAV
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_OPUS
#define MA_NO_CUSTOM
#define MA_NO_ENCODING
#define MA_NO_GENERATION

#if defined(__i386__) ||  defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
#include <immintrin.h>
#endif

#if defined(USE_OGGVORBIS)

#define MA_NO_VORBIS

#include "miniaudio.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#elif defined(USE_STB_VORBIS)

#define STB_VORBIS_NO_FAST_SCALED_FLOAT
#define STB_VORBIS_NO_INTEGER_CONVERSION

#define STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

extern void *ogg_malloc(size_t size);
extern void *ogg_calloc(size_t size, size_t n);
extern void *ogg_realloc(void *p, size_t newsize);
extern void ogg_free(void *p);

#define malloc ogg_malloc
#define calloc ogg_calloc
#define realloc ogg_realloc
#define free ogg_free

#undef STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"

#else
#error "Select an ogg/vorbis decoder implementation!"
#endif

#if defined(USE_OGGVORBIS)
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

ma_decoding_backend_vtable g_ma_decoding_backend_vtable_libvorbis = {
	ma_decoding_backend_init__libvorbis,
	ma_decoding_backend_init_file__libvorbis,
	NULL, /* onInitFileW() */
	NULL, /* onInitMemory() */
	ma_decoding_backend_uninit__libvorbis
};
#endif

#ifdef USE_LIBSAMPLERATE
#include <samplerate.h>

typedef struct
{
	SRC_STATE *state;
	double ratio;
} ma_resampler_libsamplerate;

#if 0
ma_result ma_decoding_backend_get_heap_size__libsamplerate(void *pUserData, const ma_resampler_config *pConfig, size_t *pHeapSizeInBytes)
{
}
#endif

ma_result ma_decoding_backend_init__libsamplerate(void *pUserData, const ma_resampler_config *pConfig, void *pHeap, ma_resampling_backend **ppBackend)
{
	int err;
	ma_resampler_libsamplerate *pResampler;

	assert(pConfig->sampleRateIn > 0);
	assert(pConfig->sampleRateOut > 0);

	pResampler = (ma_resampler_libsamplerate *)malloc(sizeof(ma_resampler_libsamplerate));
	pResampler->ratio = (double)pConfig->sampleRateOut / (double)pConfig->sampleRateIn;
	pResampler->state = src_new(SRC_SINC_FASTEST, pConfig->channels, &err);
	if (!pResampler->state || err) {
		free(pResampler);
		pResampler = NULL;
		*ppBackend = NULL;
		return MA_ERROR;
	}
	*ppBackend = (ma_resampling_backend *)pResampler;

	return MA_SUCCESS;
}

void ma_decoding_backend_uninit__libsamplerate(void *pUserData, ma_resampling_backend *pBackend, const ma_allocation_callbacks *pAllocationCallbacks)
{
	ma_resampler_libsamplerate *pResampler = (ma_resampler_libsamplerate *)pBackend;
	assert(pResampler->state);
	src_delete(pResampler->state);
	free(pResampler);
}

ma_result ma_decoding_backend_process__libsamplerate(void *pUserData, ma_resampling_backend *pBackend, const void *pFramesIn, ma_uint64 *pFrameCountIn, void *pFramesOut, ma_uint64 *pFrameCountOut)
{
	ma_resampler_libsamplerate *pResampler = (ma_resampler_libsamplerate *)pBackend;
	int err;

	SRC_DATA data = {0};
	data.data_in = (const float *)pFramesIn;
	data.data_out = (float *)pFramesOut;
	data.input_frames = *pFrameCountIn;
	data.output_frames = *pFrameCountOut;
	data.src_ratio = pResampler->ratio;

	err = src_process(pResampler->state, &data);
	if (err != 0)
		return MA_ERROR;

	*pFrameCountOut = data.output_frames_gen;
	*pFrameCountIn = data.input_frames_used;

	return MA_SUCCESS;
}

ma_result ma_decoding_backend_set_rate__libsamplerate(void *pUserData, ma_resampling_backend *pBackend, ma_uint32 sampleRateIn, ma_uint32 sampleRateOut)
{
	ma_resampler_libsamplerate *pResampler = (ma_resampler_libsamplerate *)pBackend;
	pResampler->ratio = (double)sampleRateOut / (double)sampleRateIn;
	return MA_SUCCESS;
}

#if 0
ma_result ma_decoding_backend_get_input_latency__libsamplerate(void *pUserData, ma_resampling_backend *pBackend)
{
}

ma_result ma_decoding_backend_get_output_latency__libsamplerate(void *pUserData, ma_resampling_backend *pBackend)
{
}

ma_result ma_decoding_backend_get_required_input_frame_count__libsamplerate(void *pUserData, ma_resampling_backend *pBackend, ma_uint64 outputFrameCount, ma_uint64* pInputFrameCount)
{
}

ma_result ma_decoding_backend_get_expected_output_frame_count__libsamplerate(void *pUserData, ma_resampling_backend *pBackend, ma_uint64 inputFrameCount, ma_uint64* pOutputFrameCount)
{
}
#endif

ma_result ma_decoding_backend_reset__libsamplerate(void *pUserData, ma_resampling_backend *pBackend)
{
	ma_resampler_libsamplerate *pResampler = (ma_resampler_libsamplerate *)pBackend;
	int rv = src_reset(pResampler->state);
	if (rv != 0)
		return MA_ERROR;
	return MA_SUCCESS;
}

ma_resampling_backend_vtable g_ma_resampling_backend_vtable_libsamplerate = {
#if 0
	ma_decoding_backend_get_heap_size__libsamplerate,
#else
	NULL, /* onGetHeapSize */
#endif
	ma_decoding_backend_init__libsamplerate,
	ma_decoding_backend_uninit__libsamplerate,
	ma_decoding_backend_process__libsamplerate,
	ma_decoding_backend_set_rate__libsamplerate,
#if 0
	ma_decoding_backend_get_input_latency__libsamplerate,
	ma_decoding_backend_get_output_latency__libsamplerate,
	ma_decoding_backend_get_required_input_frame_count__libsamplerate,
	ma_decoding_backend_get_expected_output_frame_count__libsamplerate,
#else
	NULL, /* onGetInputLatency */
	NULL, /* onGetOutputLatency */
	NULL, /* onGetRequiredInputFrameCount */
	NULL, /* onGetExpectedOutputFrameCount */
#endif
	ma_decoding_backend_reset__libsamplerate,
};
#endif
