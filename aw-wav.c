
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

#include "aw-wav.h"

int wav_parse(struct wav_info *info, const void *data) {
	const struct wav_chunk *chunk = data;
	const struct wav_riff *riff = (const void *) &chunk[1];
	const struct wav_format *format;
	u32 chunk_id;
	u32 chunk_size;

	if (btoh32(chunk->id) != wav_fourcc('R', 'I', 'F', 'F'))
		return -1;

	if (btoh32(riff->format) != wav_fourcc('W', 'A', 'V', 'E'))
		return -2;

	chunk = (const void *) &riff[1];

	for (;;) {
		chunk_id = btoh32(chunk->id);
		chunk_size = ltoh32(chunk->size);

		if (chunk_id == wav_fourcc('f', 'm', 't', ' '))
			format = (const void *) &chunk[1];
		else if (chunk_id == wav_fourcc('d', 'a', 't', 'a'))
			break;

		chunk = (const void *) ((const u8 *) &chunk[1] + chunk_size);
	}

	info->blocks = &chunk[1];
	info->size = chunk_size;
	info->sample_rate = ltoh32(format->sample_rate);
	info->sample_format = ltoh16(format->format);
	info->channel_count = ltoh16(format->channel_count);

	info->frame_count = chunk_size / info->channel_count / ltoh16(format->bits_per_sample);

	if (info->sample_format != WAV_FORMAT_INT16 &&
			info->sample_format != WAV_FORMAT_FLOAT32)
		return -3;

	return 0;
}

int wav_write(void *buffer, const struct wav_info *info) {
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
	chunk->id = htob32(wav_fourcc('R', 'I', 'F', 'F'));
	chunk->size = htol32(36 + info->size);

	riff = (void *) &chunk[1];
	riff->format = htob32(wav_fourcc('W', 'A', 'V', 'E'));

	chunk = (void *) &riff[1];
	chunk->id = htob32(wav_fourcc('f', 'm', 't', ' '));
	chunk->size = 16;

	format = (void *) &chunk[1];
	format->format = htol16((u16) info->sample_format);
	format->channel_count = htol16((u16) info->channel_count);
	format->sample_rate = htol32((u32) info->sample_rate);
	format->byte_rate = htol32((u32) (info->sample_rate * nbytes * info->channel_count));
	format->block_align = htol16((u16) (nbytes * info->channel_count));
	format->bits_per_sample = htol16(nbytes * 8);

	chunk = (void *) &format[1];
	chunk->id = htob32(wav_fourcc('d', 'a', 't', 'a'));
	chunk->size = htol32((u32)  info->size);

	return 0;
}

