#ifndef __FAST_ALECK_FAST_ALECK_H__
#define __FAST_ALECK_FAST_ALECK_H__

#include <stdbool.h>
#include <string.h>

// The configuration
typedef struct _fast_aleck_config {
	bool wrap_amps;
	bool wrap_caps;
	bool wrap_quotes;
	bool widont;
} fast_aleck_config;

// The FSM state (private)
enum _fa_fsm_state
{
	_fa_fsm_state_start,
	_fa_fsm_state_dot,
	_fa_fsm_state_dotdot,
	_fa_fsm_state_dash,
	_fa_fsm_state_dashdash,
	_fa_fsm_state_tag_start,
	_fa_fsm_state_tag,
	_fa_fsm_state_cdata,
	_fa_fsm_state_attr_squo,
	_fa_fsm_state_attr_dquo
};

// The state (private)
typedef struct _fast_aleck_state {
	fast_aleck_config config;

	enum _fa_fsm_state fsm_state;

	bool off;

	bool in_code;
	bool in_kbd;
	bool in_pre;
	bool in_script;
	bool in_samp;
	bool in_var;
	bool in_math;
	bool in_textarea;
	bool in_title;

	bool end_tag_slash_detected;
	bool is_at_start_of_run;
	bool caps_found;
	bool letter_found;
	bool chars_found_after_space;

	char *out_first_space;
	char *out_first_real_space;
	char *out_last_space;
	char *out_last_real_space;
	char *out_first_caps;
	char *out_last_caps;
	char *out_last_char;
	char *out_last_char_before_space;
	char *out_last_char_after_space;
} fast_aleck_state;

// Initialize the given configuration to the default parameters (all options
// turned off).
//
// params:
//   fast_aleck_config *config - the config to initialize
void fast_aleck_config_init(fast_aleck_config *config);

// params:
//   char *in       - the unenhanced input string
//   size_t in_size - the size, in bytes, of the unenhanced input string
//   size_t *ao_len - a pointer to where the size of the returned string should be written (can be NULL)
//
// return:
//   the enhanced string, allocated on the heap
char *fast_aleck(fast_aleck_config config, char *in, size_t in_size, size_t *ao_len);

#endif
