#include <fast-aleck/buffer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fast_aleck_buffer_create(fast_aleck_buffer *b, size_t size)
{
	b->start = malloc(size);
	if (!b->start)
	{
		perror("malloc");
		exit(1);
	}
	b->cur = b->start;
	b->size = size;
}

inline void fast_aleck_buffer_destroy(fast_aleck_buffer *b)
{
	free(b->start);
}

void fast_aleck_buffer_ensure_remaining(fast_aleck_buffer *b, size_t remaining)
{
	size_t new_size = b->size;
	while (b->start + new_size - b->cur < remaining)
		new_size *= 2;	
	if (new_size == b->size)
		return;

	char *start_orig = b->start;
	b->start = realloc(b->start, new_size);
	if (!b->start)
	{
		perror("realloc");
		exit(1);
	}
	b->size = new_size;
	b->cur = b->cur + (b->start - start_orig);
}

void fast_aleck_buffer_clear(fast_aleck_buffer *b)
{
	b->cur = b->start;
}

void fast_aleck_buffer_append_char(fast_aleck_buffer *b, char c)
{
	fast_aleck_buffer_ensure_remaining(b, 1);
	*b->cur++ = c;
}

void fast_aleck_buffer_append_string(fast_aleck_buffer *b, char *s, size_t length)
{
	fast_aleck_buffer_ensure_remaining(b, length);
	memcpy(b->cur, s, length);
	b->cur += length;
}
