#include <fast-aleck/fast-aleck.h>

#include <fast-aleck/tokenizer.h>
#include <fast-aleck/text-processor.h>
#include <fast-aleck/caps-processor.h>
#include <fast-aleck/widont-processor.h>

#include <string.h> // bzero
#include <stdlib.h> // malloc

void fast_aleck_config_init(fast_aleck_config *config)
{
	bzero(config, sizeof (fast_aleck_config));
}

char *fast_aleck(fast_aleck_config a_config, char *a_in, size_t a_in_size, size_t *ao_out_size) {
	// create state
	fa_state state;
	fa_tokenizer_state_init(&state.tokenizer_state, a_in, a_in_size);
	fa_text_processor_state_init(&state.text_processor_state);
	fa_caps_processor_state_init(&state.caps_processor_state);
	fa_widont_processor_state_init(&state.widont_processor_state);
	fa_token_buffer_init(&state.token_buffer, a_in_size/16); // 16 is a random guess

	// run
	fa_tokenizer_run(&state);

	// convert to string
	size_t length = fa_token_buffer_get_length(&state.token_buffer);
	char *s = malloc(length+1);
	fa_token_buffer_write(&state.token_buffer, s);

	// done
	if (ao_out_size)
		*ao_out_size = length;
	return s;
}
