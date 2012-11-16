#include <fast-aleck/widont-processor.h>

static inline void fa_widont_processor_pass_on_token(fa_state *state, fa_token token) {
	fa_token_buffer_append(&state->token_buffer, token);
}

void fa_widont_processor_state_init(fa_widont_processor_state *state) {
}

void fa_widont_processor_handle_token(fa_state *state, fa_token token) {
	// TODO implement
	fa_widont_processor_pass_on_token(state, token);
}
