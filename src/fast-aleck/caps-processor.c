#include <fast-aleck/caps-processor.h>

#include <fast-aleck/widont-processor.h>

static inline void fa_caps_processor_pass_on_token(fa_state *state, fa_token token) {
	fa_widont_processor_handle_token(state, token);
}

void fa_caps_processor_state_init(fa_caps_processor_state *state) {
}

void fa_caps_processor_handle_token(fa_state *state, fa_token token) {
	// TODO implement
	fa_caps_processor_pass_on_token(state, token);
}
