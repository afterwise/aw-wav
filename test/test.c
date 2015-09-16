
#define WAV_WRITE 1

#include "aw-wav.h"
#include "aw-fs.h"

#if __APPLE__
# include <OpenAL/al.h>
# include <OpenAL/alc.h>
#else
# include <AL/al.h>
# include <AL/alc.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static struct wav_info wav_info;

static int load(const char *path) {
	struct fs_map map;
	void *ptr;
	int err;

	ptr = fs_map(&map, path);

	if ((err = wav_parse(&wav_info, ptr)) < 0)
		return fprintf(stderr, "Parse failed: %d\n", err), -1;

	return 0;
}

#define BUFFER_SIZE (16384)
static int64_t offset;

intptr_t wav_fd;

static void queue(unsigned src, unsigned buf) {
	unsigned frame_count = BUFFER_SIZE / (wav_info.channel_count * sizeof (int16_t));
	void *buffer;
	size_t size;

	if (frame_count > wav_info.frame_count - offset)
		frame_count = wav_info.frame_count - offset;

        buffer = (int16_t *) wav_info.blocks + offset * wav_info.channel_count;
	offset += frame_count;
	size = frame_count * wav_info.channel_count * sizeof (int16_t);

	if (size > 0) {
		alBufferData(
			buf, AL_FORMAT_STEREO16,
			buffer, size, wav_info.sample_rate);

		alSourceQueueBuffers(src, 1, &buf);

		if (wav_fd > 0)
			fs_write(wav_fd, buffer, size);
	}
}

static void play() {
	ALCdevice *dev;
	ALCcontext *ctx;
	unsigned src;
	unsigned bufs[2];
	unsigned buf;
	int v;
	float zero[3];

	memset(zero, 0, sizeof zero);

	dev = alcOpenDevice(NULL);
	ctx = alcCreateContext(dev, NULL);
	alcMakeContextCurrent(ctx);

	alGenSources(1, &src);
	alGenBuffers(2, bufs);

	alSourcef(src, AL_PITCH, 1.0f);
	alSourcef(src, AL_GAIN, 1.0f);
	alSourcefv(src, AL_POSITION, zero);
	alSourcefv(src, AL_VELOCITY, zero);
	alSourcei(src, AL_SOURCE_RELATIVE, AL_TRUE);
	alSourcei(src, AL_LOOPING, AL_FALSE);

	queue(src, bufs[0]);
	queue(src, bufs[1]);

	alSourcePlay(src);

	for (;;) {
		alGetSourcei(src, AL_BUFFERS_PROCESSED, &v);

		while (v-- > 0) {
			alSourceUnqueueBuffers(src, 1, &buf);
			queue(src, buf);
		}

		alGetSourcei(src, AL_SOURCE_STATE, &v);

		if (v == AL_STOPPED)
			break;

		usleep(1000);
	}

        alDeleteSources(1, &src);
        alDeleteBuffers(2, bufs);
        alcMakeContextCurrent(NULL);
        alcDestroyContext(ctx);
        alcCloseDevice(dev);
}

int main(int argc, char *argv[]) {
	uint32_t wav_buf[WAV_HEADER_SIZE / 4 + 1];
	struct wav_info out_info;
	off_t file_end, data_off;

	char *caf = NULL;
	char *wav = NULL;

	if (argc > 1)
		caf = argv[1];

	if (argc > 2)
		wav = argv[2];

	if (caf == NULL) {
		fputs("Usage: test <caf> [wav]\n", stderr);
		return 1;
	}

	if (load(caf) < 0)
		return 1;

	if (wav != NULL) {
		if ((wav_fd = fs_open(wav, FS_CREAT | FS_TRUNC | FS_WRONLY)) < 0)
			perror("Open failed");
		else {
			fs_write(wav_fd, wav_buf, WAV_HEADER_SIZE);
			data_off = fs_seek(wav_fd, 0, FS_SEEK_CUR);
			assert(data_off == WAV_HEADER_SIZE);
		}
	}

	play();

	if (wav_fd > 0) {
		file_end = fs_seek(wav_fd, 0, FS_SEEK_CUR);
		fs_seek(wav_fd, 0, FS_SEEK_SET);

		out_info.blocks = NULL;
		out_info.size = file_end - data_off;
		out_info.sample_rate = wav_info.sample_rate;
		out_info.frame_count = wav_info.frame_count;
		out_info.sample_format = WAV_FORMAT_INT16;
		out_info.channel_count = wav_info.channel_count;

		wav_write(wav_buf, &out_info);
		fs_write(wav_fd, wav_buf, WAV_HEADER_SIZE);

		fs_close(wav_fd);
	}

	return 0;
}

