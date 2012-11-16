#ifndef __FAST_ALECK_TOKENIZER_H__
#define __FAST_ALECK_TOKENIZER_H__

#include <fast-aleck/state.h>

void fa_tokenizer_state_init(fa_tokenizer_state *state);
void fa_tokenizer_feed(fa_state *state, char *input, size_t input_length);

#endif
