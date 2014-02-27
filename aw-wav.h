
#ifndef AW_WAV_H
#define AW_WAV_H

#include <stdint.h>
#if __linux__
# include <endian.h>
#elif __APPLE__
# include <libkern/OSByteOrder.h>
# define htobe16 OSSwapHostToBigInt16
# define htole16 OSSwapHostToLittleInt16
# define be16toh OSSwapBigToHostInt16
# define le16toh OSSwapLittleToHostInt16
# define htobe32 OSSwapHostToBigInt32
# define htole32 OSSwapHostToLittleInt32
# define be32toh OSSwapBigToHostInt32
# define le32toh OSSwapLittleToHostInt32
# define htobe64 OSSwapHostToBigInt64
# define htole64 OSSwapHostToLittleInt64
# define be64toh OSSwapBigToHostInt64
# define le64toh OSSwapLittleToHostInt64
#endif

#if __GNUC__
# define _wav_alwaysinline inline __attribute__((always_inline, nodebug))
# define _wav_packed __attribute__((packed))
#elif _MSC_VER
# define _wav_alwaysinline __forceinline
# define _wav_packed
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define WAV_FORMAT_INT16 (1)
#define WAV_FORMAT_FLOAT32 (3)

#define WAV_HEADER_SIZE (44)

#if _MSC_VER
# pragma pack(push, 1)
#endif

struct wav_info {
	const void *blocks;
	uint64_t size;
	double sample_rate;
	uint64_t frame_count;
	unsigned sample_format;
	unsigned channel_count;
};

struct _wav_packed wav_chunk {
	uint32_t id;
	uint32_t size;
};

struct _wav_packed wav_riff {
        uint32_t format;
};

struct _wav_packed wav_format {
        uint16_t format;
        uint16_t channel_count;
        uint32_t sample_rate;
        uint32_t byte_rate;
        uint16_t block_align;
        uint16_t bits_per_sample;
};

#if _MSC_VER
# pragma pack(pop)
#endif

static int wav_parse(struct wav_info *info, const void *data) {
	const struct wav_chunk *chunk = data;
	const struct wav_riff *riff = (const void *) &chunk[1];
	const struct wav_format *format;
	uint32_t chunk_id;
	uint32_t chunk_size;

	if (be32toh(chunk->id) != 'RIFF')
		return -1;

	if (be32toh(riff->format) != 'WAVE')
		return -2;

	chunk = (const void *) &riff[1];

	for (;;) {
		chunk_id = be32toh(chunk->id);
		chunk_size = le32toh(chunk->size);

		if (chunk_id == 'fmt ')
			format = (const void *) &chunk[1];
		else if (chunk_id == 'data')
			break;

		chunk = (const void *) ((const uint8_t *) &chunk[1] + chunk_size);
	}

	info->blocks = &chunk[1];
	info->size = chunk_size;
	info->sample_rate = le32toh(format->sample_rate);
	info->sample_format = le16toh(format->format);
	info->channel_count = le16toh(format->channel_count);

	info->frame_count = chunk_size / info->channel_count / le16toh(format->bits_per_sample);

	if (info->sample_format != WAV_FORMAT_INT16 &&
			info->sample_format != WAV_FORMAT_FLOAT32)
		return -3;

	return 0;
}

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
	chunk->id = htobe32('RIFF');
	chunk->size = htole32(36 + info->size);

	riff = (void *) &chunk[1];
	riff->format = htobe32('WAVE');

	chunk = (void *) &riff[1];
	chunk->id = htobe32('fmt ');
	chunk->size = 16;

	format = (void *) &chunk[1];
	format->format = htole16((uint16_t) info->sample_format);
	format->channel_count = htole16((uint16_t) info->channel_count);
	format->sample_rate = htole32((uint32_t) info->sample_rate);
	format->byte_rate = htole32((uint32_t) (info->sample_rate * nbytes * info->channel_count));
	format->block_align = htole16((uint16_t) (nbytes * info->channel_count));
	format->bits_per_sample = htole16(nbytes * 8);

	chunk = (void *) &format[1];
	chunk->id = htobe32('data');
	chunk->size = htole32((uint32_t)  info->size);

	return 0;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* AW_WAV_H */

