
export CFLAGS += -Wall -Wextra

test: test.o aw-fs/libaw-fs.a
	$(CC) $(LDFLAGS) -framework OpenAL -o $@ $^

%.o: %.c ../aw-wav.h aw-fs/aw-fs.h
	$(CC) $(CFLAGS) -I.. -Iaw-fs -c $< -o $@

aw-fs/libaw-fs.a:
	$(MAKE) -Caw-fs

aw-fs/aw-fs.h:
	git clone git@github.com:afterwise/aw-fs.git && \
	touch -c $@

.PHONY: clean
clean:
	rm -f test test.o
	rm -rf aw-bytes aw-fs

