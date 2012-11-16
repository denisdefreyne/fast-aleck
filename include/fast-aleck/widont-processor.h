#ifndef __FAST_ALECK_WIDONT_PROCESSOR_H__
#define __FAST_ALECK_WIDONT_PROCESSOR_H__

#include <fast-aleck/state.h>

void fa_widont_processor_state_init(fa_widont_processor_state *state);
void fa_widont_processor_handle_token(fa_state *state, fa_token token);

#endif
