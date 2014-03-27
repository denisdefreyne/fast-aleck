#ifndef __FAST_ALECK_STATE_H__
#define __FAST_ALECK_STATE_H__

#include <fast-aleck/fast-aleck.h>
#include <fast-aleck/token.h>
#include <fast-aleck/token-buffer.h>

// Tokenizer state

enum _fa_tokenizer_fsm_state {
	fa_tokenizer_fsm_state_entry,
	fa_tokenizer_fsm_state_tag_start,       // <
	fa_tokenizer_fsm_state_tag_name,
	fa_tokenizer_fsm_state_ltexcl,          // <!
	fa_tokenizer_fsm_state_comment_start_1, // <!-
	fa_tokenizer_fsm_state_comment,         // <!--
	fa_tokenizer_fsm_state_comment_end_1,   // -
	fa_tokenizer_fsm_state_comment_end_2,   // --
	fa_tokenizer_fsm_state_cdata_start_1,   // <![
	fa_tokenizer_fsm_state_cdata_start_2,   // <![C
	fa_tokenizer_fsm_state_cdata_start_3,   // <![CD
	fa_tokenizer_fsm_state_cdata_start_4,   // <![CDA
	fa_tokenizer_fsm_state_cdata_start_5,   // <![CDAT
	fa_tokenizer_fsm_state_cdata_start_6,   // <![CDATA
	fa_tokenizer_fsm_state_cdata,           // <![CDATA[
	fa_tokenizer_fsm_state_cdata_end_1,     // ]
	fa_tokenizer_fsm_state_cdata_end_2,     // ]]
	fa_tokenizer_fsm_state_attr,
	fa_tokenizer_fsm_state_attr_squo,
	fa_tokenizer_fsm_state_attr_dquo
};
typedef enum _fa_tokenizer_fsm_state fa_tokenizer_fsm_state;

struct _fa_tokenizer_state {
	fa_slice               input;
	fa_token               current_token;
	fa_tokenizer_fsm_state fsm_state;
	fa_slice               tag_name;
	bool                   is_closing_tag;
	bool                   is_in_excluded_element;
	bool                   is_in_code;
	bool                   is_in_kbd;
	bool                   is_in_pre;
	bool                   is_in_script;
	bool                   is_in_samp;
	bool                   is_in_var;
	bool                   is_in_math;
	bool                   is_in_textarea;
	bool                   is_in_title;
	fa_token_buffer        *_token_buffer; // for automated testing only
};
typedef struct _fa_tokenizer_state fa_tokenizer_state;

// Text processor state

enum _fa_text_processor_fsm_state {
	fa_text_processor_fsm_state_start,
	fa_text_processor_fsm_state_dot,
	fa_text_processor_fsm_state_dotdot,
	fa_text_processor_fsm_state_dash,
	fa_text_processor_fsm_state_dashdash,
	fa_text_processor_fsm_state_amp,
	fa_text_processor_fsm_state_ampa,
	fa_text_processor_fsm_state_ampam,
	fa_text_processor_fsm_state_ampamp
};
typedef enum _fa_text_processor_fsm_state fa_text_processor_fsm_state;

struct _fa_text_processor_state {
	fa_text_processor_fsm_state fsm_state;
	bool                        is_at_start_of_run;
	char                        last_char;
};
typedef struct _fa_text_processor_state fa_text_processor_state;

// Caps processor state

struct _fa_caps_processor_state {
	fa_token normal_token;
	fa_token caps_token;
	bool     letter_found;
};
typedef struct _fa_caps_processor_state fa_caps_processor_state;

// Widont state

struct _fa_widont_processor_state {
	fa_token_buffer words1;
	fa_token_buffer spaces1;
	fa_token_buffer words2;
	fa_token_buffer spaces2;
	bool have_words;
	bool expect_word;
};
typedef struct _fa_widont_processor_state fa_widont_processor_state;

// Combined state

struct _fa_state {
	fast_aleck_config         config;
	fa_tokenizer_state        tokenizer_state;
	fa_text_processor_state   text_processor_state;
	fa_caps_processor_state   caps_processor_state;
	fa_widont_processor_state widont_processor_state;
	fa_token_buffer           token_buffer;
};
typedef struct _fa_state fa_state;

#endif
