#ifndef __FAST_ALECK_FAST_ALECK_H__
#define __FAST_ALECK_FAST_ALECK_H__

#include <stdbool.h>
#include <string.h>

#include <fast-aleck/buffer.h>

// The configuration
typedef struct _fast_aleck_config {
	bool wrap_amps;
	bool wrap_caps;
	bool wrap_quotes;
	bool widont;
} fast_aleck_config;

// The FSM tag state (private)
enum _fa_fsm_tag_state {
	_fa_fsm_tag_state_entry,
	_fa_fsm_tag_state_tag_start, // <
	_fa_fsm_tag_state_tag_name,
	_fa_fsm_tag_state_ltexcl,          // <!
	_fa_fsm_tag_state_comment_start_1, // <!-
	_fa_fsm_tag_state_comment,         // <!--
	_fa_fsm_tag_state_comment_end_1,   // -
	_fa_fsm_tag_state_comment_end_2,   // --
	_fa_fsm_tag_state_cdata_start_1,   // <![
	_fa_fsm_tag_state_cdata_start_2,   // <![C
	_fa_fsm_tag_state_cdata_start_3,   // <![CD
	_fa_fsm_tag_state_cdata_start_4,   // <![CDA
	_fa_fsm_tag_state_cdata_start_5,   // <![CDAT
	_fa_fsm_tag_state_cdata_start_6,   // <![CDATA
	_fa_fsm_tag_state_cdata,           // <![CDATA[
	_fa_fsm_tag_state_cdata_end_1,     // ]
	_fa_fsm_tag_state_cdata_end_2,     // ]]
	_fa_fsm_tag_state_attr,
	_fa_fsm_tag_state_attr_squo,
	_fa_fsm_tag_state_attr_dquo
};

// The FSM text state (private)
enum _fa_fsm_text_state
{
	_fa_fsm_text_state_start,
	_fa_fsm_text_state_dot,
	_fa_fsm_text_state_dotdot,
	_fa_fsm_text_state_dash,
	_fa_fsm_text_state_dashdash,
	_fa_fsm_text_state_amp,
	_fa_fsm_text_state_ampa,
	_fa_fsm_text_state_ampam,
	_fa_fsm_text_state_ampamp

};

// The state (private)
typedef struct _fast_aleck_state {
	fast_aleck_config config;

	// tag state
	enum _fa_fsm_tag_state fsm_tag_state;
	bool is_closing_tag;
	fast_aleck_buffer tag_name;
	bool is_in_excluded_element;
	bool is_in_code;
	bool is_in_kbd;
	bool is_in_pre;
	bool is_in_script;
	bool is_in_samp;
	bool is_in_var;
	bool is_in_math;
	bool is_in_textarea;
	bool is_in_title;

	// text state
	enum _fa_fsm_text_state fsm_state;
	bool is_at_start_of_run;
	char last_char;

	// caps state
	fast_aleck_buffer caps_buf;
	bool char_found;

	// widont state
	fast_aleck_buffer widont_buf;
	bool              widont_buf_starts_with_space;
	bool              widont_buf_ends_with_space;
	bool              widont_has_preceding_chars;
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
char *fast_aleck(fast_aleck_config a_config, char *a_in, size_t a_in_size, size_t *ao_out_size);

// TODO document
void fast_aleck_init(fast_aleck_state *a_state, fast_aleck_config a_config);

// TODO document
void fast_aleck_feed(fast_aleck_state *a_state, char *a_in, size_t a_in_size, fast_aleck_buffer *out_buf);

// TODO document
void fast_aleck_finish(fast_aleck_state *state, fast_aleck_buffer *buf);

#endif
