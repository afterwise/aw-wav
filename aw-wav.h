
/*
   Copyright (c) 2014 Malte Hildingsson, malte (at) afterwi.se

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
 */

#ifndef AW_WAV_H
#define AW_WAV_H

#if __GNUC__
# define _wav_packed __attribute__((packed))
#elif _MSC_VER
# define _wav_packed
#endif

#if __GNUC__
# define _wav_alwaysinline inline __attribute__((always_inline))
# define _wav_unused __attribute__((__unused__))
#elif _MSC_VER
# define _wav_alwaysinline __forceinline
# define _wav_unused
#endif

#define wav_fourcc(a,b,c,d) \
        ((wav_u32_t) (wav_u8_t) (d) | ((wav_u32_t) (wav_u8_t) (c) << 8) | \
        ((wav_u32_t) (wav_u8_t) (b) << 16) | ((wav_u32_t) (wav_u8_t) (a) << 24))

#ifdef __cplusplus
extern "C" {
#endif

#define WAV_FORMAT_INT16 (0x1)
#define WAV_FORMAT_FLOAT32 (0x3)
#define WAV_FORMAT_IMA (0x11)

#define WAV_HEADER_SIZE (44)

typedef signed char wav_s8_t;
typedef unsigned char wav_u8_t;
typedef signed short wav_s16_t;
typedef unsigned short wav_u16_t;
typedef signed int wav_s32_t;
typedef unsigned int wav_u32_t;
#if _MSC_VER
typedef signed __int64 wav_s64_t;
typedef unsigned __int64 wav_u64_t;
#else
typedef signed long long wav_s64_t;
typedef unsigned long long wav_u64_t;
#endif
typedef float wav_f32_t;
typedef double wav_f64_t;

struct wav_info {
	const void *blocks;
	wav_u64_t size;
	wav_f64_t sample_rate;
	wav_u64_t frame_count;
	wav_u32_t channel_count;
	wav_u32_t sample_format;
};

#if _MSC_VER
# pragma pack(push, 1)
#endif

struct _wav_packed wav_chunk {
	wav_u32_t id;
	wav_u32_t size;
};

struct _wav_packed wav_riff {
	wav_u32_t format;
};

struct _wav_packed wav_format {
	wav_u16_t format;
	wav_u16_t channel_count;
	wav_u32_t sample_rate;
	wav_u32_t byte_rate;
	wav_u16_t block_align;
	wav_u16_t bits_per_sample;
};

#if _MSC_VER
# pragma pack(pop)
#endif

static _wav_alwaysinline wav_u16_t wav_bswap16(wav_u16_t v) {
        return
                (v << 0x08 & 0xff00u) |
                (v >> 0x08 & 0x00ffu);
}

static _wav_alwaysinline wav_u32_t wav_bswap32(wav_u32_t v) {
        return
                (v << 0x18 & 0xff000000ul) |
                (v << 0x08 & 0x00ff0000ul) |
                (v >> 0x08 & 0x0000ff00ul) |
                (v >> 0x18 & 0x000000fful);
}

static _wav_alwaysinline wav_u64_t wav_bswap64(wav_u64_t v) {
        return
                (v << 0x38 & 0xff00000000000000ull) |
                (v << 0x28 & 0x00ff000000000000ull) |
                (v << 0x18 & 0x0000ff0000000000ull) |
                (v << 0x08 & 0x000000ff00000000ull) |
                (v >> 0x08 & 0x00000000ff000000ull) |
                (v >> 0x18 & 0x0000000000ff0000ull) |
                (v >> 0x28 & 0x000000000000ff00ull) |
                (v >> 0x38 & 0x00000000000000ffull);
}

#if __BIG_ENDIAN__ || __ARMEB__
static _wav_alwaysinline wav_u16_t wav_btoh16(wav_u16_t v) { return v; }
static _wav_alwaysinline wav_u32_t wav_btoh32(wav_u32_t v) { return v; }
static _wav_alwaysinline wav_u64_t wav_btoh64(wav_u64_t v) { return v; }

static _wav_alwaysinline wav_u16_t wav_ltoh16(wav_u16_t v) { return wav_bswap16(v); }
static _wav_alwaysinline wav_u32_t wav_ltoh32(wav_u32_t v) { return wav_bswap32(v); }
static _wav_alwaysinline wav_u64_t wav_ltoh64(wav_u64_t v) { return wav_bswap64(v); }

static _wav_alwaysinline wav_u16_t wav_htob16(wav_u16_t v) { return v; }
static _wav_alwaysinline wav_u32_t wav_htob32(wav_u32_t v) { return v; }
static _wav_alwaysinline wav_u64_t wav_htob64(wav_u64_t v) { return v; }

static _wav_alwaysinline wav_u16_t wav_htol16(wav_u16_t v) { return wav_bswap16(v); }
static _wav_alwaysinline wav_u32_t wav_htol32(wav_u32_t v) { return wav_bswap32(v); }
static _wav_alwaysinline wav_u64_t wav_htol64(wav_u64_t v) { return wav_bswap64(v); }
#else
static _wav_alwaysinline wav_u16_t wav_btoh16(wav_u16_t v) { return wav_bswap16(v); }
static _wav_alwaysinline wav_u32_t wav_btoh32(wav_u32_t v) { return wav_bswap32(v); }
static _wav_alwaysinline wav_u64_t wav_btoh64(wav_u64_t v) { return wav_bswap64(v); }

static _wav_alwaysinline wav_u16_t wav_ltoh16(wav_u16_t v) { return v; }
static _wav_alwaysinline wav_u32_t wav_ltoh32(wav_u32_t v) { return v; }
static _wav_alwaysinline wav_u64_t wav_ltoh64(wav_u64_t v) { return v; }

static _wav_alwaysinline wav_u16_t wav_htob16(wav_u16_t v) { return wav_bswap16(v); }
static _wav_alwaysinline wav_u32_t wav_htob32(wav_u32_t v) { return wav_bswap32(v); }
static _wav_alwaysinline wav_u64_t wav_htob64(wav_u64_t v) { return wav_bswap64(v); }

static _wav_alwaysinline wav_u16_t wav_htol16(wav_u16_t v) { return v; }
static _wav_alwaysinline wav_u32_t wav_htol32(wav_u32_t v) { return v; }
static _wav_alwaysinline wav_u64_t wav_htol64(wav_u64_t v) { return v; }
#endif

static int wav_parse(struct wav_info *info, const void *data) _wav_unused;
static int wav_parse(struct wav_info *info, const void *data) {
	const struct wav_chunk *chunk = data;
	const struct wav_riff *riff = (const void *) &chunk[1];
	const struct wav_format *format;
	wav_u32_t chunk_id;
	wav_u32_t chunk_size;

	if (wav_btoh32(chunk->id) != wav_fourcc('R', 'I', 'F', 'F'))
		return -1;

	if (wav_btoh32(riff->format) != wav_fourcc('W', 'A', 'V', 'E'))
		return -2;

	chunk = (const void *) &riff[1];

	for (;;) {
		chunk_id = wav_btoh32(chunk->id);
		chunk_size = wav_ltoh32(chunk->size);

		if (chunk_id == wav_fourcc('f', 'm', 't', ' '))
			format = (const void *) &chunk[1];
		else if (chunk_id == wav_fourcc('d', 'a', 't', 'a'))
			break;

		chunk = (const void *) ((const wav_u8_t *) &chunk[1] + chunk_size);
	}

	info->blocks = &chunk[1];
	info->size = chunk_size;
	info->sample_rate = wav_ltoh32(format->sample_rate);
	info->sample_format = wav_ltoh16(format->format);
	info->channel_count = wav_ltoh16(format->channel_count);

	info->frame_count = chunk_size / info->channel_count / (wav_ltoh16(format->bits_per_sample) / 8);

	return 0;
}

static int wav_write(void *buffer, const struct wav_info *info) _wav_unused;
static int wav_write(void *buffer, const struct wav_info *info) {
	struct wav_chunk *chunk;
	struct wav_riff *riff;
	struct wav_format *format;
	unsigned nbytes;

	if (info->sample_format == WAV_FORMAT_INT16)
		nbytes = 2;
	else if (info->sample_format == WAV_FORMAT_FLOAT32)
		nbytes = 4;
	else
		return -1;

	chunk = buffer;
	chunk->id = wav_htob32(wav_fourcc('R', 'I', 'F', 'F'));
	chunk->size = wav_htol32(36 + info->size);

	riff = (void *) &chunk[1];
	riff->format = wav_htob32(wav_fourcc('W', 'A', 'V', 'E'));

	chunk = (void *) &riff[1];
	chunk->id = wav_htob32(wav_fourcc('f', 'm', 't', ' '));
	chunk->size = 16;

	format = (void *) &chunk[1];
	format->format = wav_htol16((wav_u16_t) info->sample_format);
	format->channel_count = wav_htol16((wav_u16_t) info->channel_count);
	format->sample_rate = wav_htol32((wav_u32_t) info->sample_rate);
	format->byte_rate = wav_htol32((wav_u32_t) (info->sample_rate * nbytes * info->channel_count));
	format->block_align = wav_htol16((wav_u16_t) (nbytes * info->channel_count));
	format->bits_per_sample = wav_htol16(nbytes * 8);

	chunk = (void *) &format[1];
	chunk->id = wav_htob32(wav_fourcc('d', 'a', 't', 'a'));
	chunk->size = wav_htol32((wav_u32_t)  info->size);

	return 0;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* AW_WAV_H */

