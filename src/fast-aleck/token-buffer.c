#include <fast-aleck/token-buffer.h>

#include <stdlib.h>
#include <string.h>

static void _fa_token_buffer_resize(fa_token_buffer *buffer) {
	buffer->capacity *= 2;
	buffer->tokens = realloc(buffer->tokens, sizeof (fa_token) * buffer->capacity);
}

void fa_token_buffer_init(fa_token_buffer *buffer, size_t expected_count) {
	buffer->capacity = expected_count+1;
	buffer->size = 0;
	buffer->tokens = malloc(sizeof (fa_token) * buffer->capacity);
	buffer->_cached_length = 0;
}

void fa_token_buffer_destroy(fa_token_buffer *buffer) {
	free(buffer->tokens);
}

void fa_token_buffer_append(fa_token_buffer *buffer, fa_token token) {
	if (buffer->capacity == buffer->size)
		_fa_token_buffer_resize(buffer);

	buffer->tokens[buffer->size] = token;
	++buffer->size;
	buffer->_cached_length += token.slice.length;
}

size_t fa_token_buffer_get_length(fa_token_buffer *buffer) {
	return buffer->_cached_length;
}

void fa_token_buffer_write(fa_token_buffer *buffer, char *string) {
	for (size_t i = 0; i < buffer->size; ++i) {
		fa_slice slice = buffer->tokens[i].slice;
		memcpy(string, slice.start, slice.length);
		string += slice.length;
	}
	*string = '\0';
}
