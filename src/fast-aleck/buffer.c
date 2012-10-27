#include <fast-aleck/buffer.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fast_aleck_buffer_create(fast_aleck_buffer *b, size_t size)
{
	assert(size > 0);

	b->start = malloc(size);
	if (!b->start)
	{
		perror("malloc");
		exit(1);
	}
	b->cur = b->start;
	b->size = size;
}

void fast_aleck_buffer_ensure_remaining(fast_aleck_buffer *b, size_t remaining)
{
	assert(remaining > 0);
	assert(b->size > 0);

	size_t new_size = b->size;
	while (b->start + new_size < b->cur + remaining)
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
