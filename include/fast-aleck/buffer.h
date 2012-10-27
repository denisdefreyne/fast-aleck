#ifndef __FAST_ALECK_BUFFER_H__
#define __FAST_ALECK_BUFFER_H__

#include <stddef.h>
#include <string.h>

typedef struct _fast_aleck_buffer fast_aleck_buffer;

struct _fast_aleck_buffer {
	char   *start;
	char   *cur;
	size_t size;
};

void fast_aleck_buffer_create(fast_aleck_buffer *b, size_t size);
inline void fast_aleck_buffer_destroy(fast_aleck_buffer *b);
void fast_aleck_buffer_ensure_remaining(fast_aleck_buffer *b, size_t remaining);

static inline void fast_aleck_buffer_clear(fast_aleck_buffer *b)
{
	b->cur = b->start;
}

static inline void fast_aleck_buffer_unchecked_append_char(fast_aleck_buffer *b, char c)
{
	*b->cur++ = c;
}

static inline void fast_aleck_buffer_unchecked_append_string(fast_aleck_buffer *b, char *s, size_t length)
{
	memcpy(b->cur, s, length);
	b->cur += length;
}

static inline void fast_aleck_buffer_append_buffer(fast_aleck_buffer *b, fast_aleck_buffer *b2)
{
	ptrdiff_t diff = b2->cur - b2->start;
	memcpy(b->cur, b2->start, diff);
	b->cur += diff;
}

static inline void fast_aleck_buffer_append_char(fast_aleck_buffer *b, char c)
{
	fast_aleck_buffer_ensure_remaining(b, 1);
	fast_aleck_buffer_unchecked_append_char(b, c);
}

static inline void fast_aleck_buffer_append_string(fast_aleck_buffer *b, char *s, size_t length)
{
	fast_aleck_buffer_ensure_remaining(b, length);
	fast_aleck_buffer_unchecked_append_string(b, s, length);
}

#endif
