#ifndef __FAST_ALECK_TEXT_PROCESSOR_H__
#define __FAST_ALECK_TEXT_PROCESSOR_H__

#include <fast-aleck/state.h>

void fa_text_processor_state_init(fa_text_processor_state *state);
void fa_text_processor_handle_token(fa_state *state, fa_token token);

#endif
