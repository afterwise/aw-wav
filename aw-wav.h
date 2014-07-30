
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

#include "aw-endian.h"

#if __GNUC__
# define _wav_packed __attribute__((packed))
#elif _MSC_VER
# define _wav_packed
#endif

#define wav_fourcc(a,b,c,d) \
        ((u32) (u8) (d) | ((u32) (u8) (c) << 8) | \
        ((u32) (u8) (b) << 16) | ((u32) (u8) (a) << 24))

#ifdef __cplusplus
extern "C" {
#endif

#define WAV_FORMAT_INT16 (1)
#define WAV_FORMAT_FLOAT32 (3)

#define WAV_HEADER_SIZE (44)

struct wav_info {
	const void *blocks;
	u64 size;
	f64 sample_rate;
	u64 frame_count;
	u32 sample_format;
	u32 channel_count;
};

#if _MSC_VER
# pragma pack(push, 1)
#endif

struct _wav_packed wav_chunk {
	u32 id;
	u32 size;
};

struct _wav_packed wav_riff {
        u32 format;
};

struct _wav_packed wav_format {
        u16 format;
        u16 channel_count;
        u32 sample_rate;
        u32 byte_rate;
        u16 block_align;
        u16 bits_per_sample;
};

#if _MSC_VER
# pragma pack(pop)
#endif

int wav_parse(struct wav_info *info, const void *data);
int wav_write(void *buffer, const struct wav_info *info);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* AW_WAV_H */

