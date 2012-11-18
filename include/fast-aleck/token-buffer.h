#ifndef __FAST_ALECK_TOKEN_BUFFER_H__
#define __FAST_ALECK_TOKEN_BUFFER_H__

#include <fast-aleck/token.h>

#include <stddef.h>

struct _fa_token_buffer {
	fa_token *tokens;
	size_t capacity;
	size_t size;
	size_t _cached_length;
};
typedef struct _fa_token_buffer fa_token_buffer;

void fa_token_buffer_init(fa_token_buffer *buffer, size_t expected_count);
void fa_token_buffer_destroy(fa_token_buffer *buffer);

void fa_token_buffer_append(fa_token_buffer *buffer, fa_token token);
size_t fa_token_buffer_count(fa_token_buffer *buffer);
fa_token fa_token_buffer_at(fa_token_buffer *buffer, size_t i);
size_t fa_token_buffer_get_length(fa_token_buffer *buffer);
void fa_token_buffer_write(fa_token_buffer *buffer, char *string);

#endif
