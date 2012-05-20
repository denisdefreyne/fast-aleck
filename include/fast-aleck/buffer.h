#ifndef __FAST_ALECK_BUFFER_H__
#define __FAST_ALECK_BUFFER_H__

#include <stddef.h>

typedef struct _fast_aleck_buffer fast_aleck_buffer;

struct _fast_aleck_buffer {
	char   *start;
	char   *cur;
	size_t size;
};

void fast_aleck_buffer_create(fast_aleck_buffer *b, size_t size);
inline void fast_aleck_buffer_destroy(fast_aleck_buffer *b);
void fast_aleck_buffer_ensure_remaining(fast_aleck_buffer *b, size_t remaining);
void fast_aleck_buffer_clear(fast_aleck_buffer *b);
void fast_aleck_buffer_append_char(fast_aleck_buffer *b, char c);
void fast_aleck_buffer_append_string(fast_aleck_buffer *b, char *s, size_t length);

#endif
