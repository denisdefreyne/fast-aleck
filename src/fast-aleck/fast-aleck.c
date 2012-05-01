#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <fast-aleck/fast-aleck.h>

enum _fa_state
{
	_fa_state_start,
	_fa_state_dot,
	_fa_state_dotdot,
	_fa_state_dash,
	_fa_state_dashdash,
	_fa_state_tag_start,
	_fa_state_tag,
	_fa_state_cdata,
	_fa_state_attr_squo,
	_fa_state_attr_dquo
};

static inline size_t _fa_write_ellipsis(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0xA6;

	return 3;
}

static inline size_t _fa_write_mdash(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0x94;

	return 3;
}

static inline size_t _fa_write_single_quote_start(char *out, fa_bool a_should_wrap)
{
	char *s1 = "<span class=\"quo\">";
	char *s2 = "</span>";

	if (a_should_wrap)
	{
		memcpy(out, s1, 18);
		out += 18;
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x98;
		memcpy(out, s2, 7);
		return 28;
	}
	else
	{
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x98;
		return 3;
	}
}

static inline size_t _fa_write_single_quote_end(char *out, fa_bool a_should_wrap)
{
	char *s1 = "<span class=\"quo\">";
	char *s2 = "</span>";

	if (a_should_wrap)
	{
		memcpy(out, s1, 18);
		out += 18;
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x99;
		memcpy(out, s2, 7);
		return 28;
	}
	else
	{
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x99;
		return 3;
	}
}

static inline size_t _fa_write_double_quote_start(char *out, fa_bool a_should_wrap)
{
	char *s1 = "<span class=\"dquo\">";
	char *s2 = "</span>";

	if (a_should_wrap)
	{
		memcpy(out, s1, 19);
		out += 19;
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x9C;
		memcpy(out, s2, 7);
		return 29;
	}
	else
	{
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x9C;
		return 3;
	}
}

static inline size_t _fa_write_double_quote_end(char *out, fa_bool a_should_wrap)
{
	char *s1 = "<span class=\"dquo\">";
	char *s2 = "</span>";

	if (a_should_wrap)
	{
		memcpy(out, s1, 19);
		out += 19;
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x9D;
		memcpy(out, s2, 7);
		return 29;
	}
	else
	{
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x9D;
		return 3;
	}
}

static inline size_t _fa_write_wrapped_amp(char *out)
{
	char *s = "<span class=\"amp\">&amp;</span>";
	memcpy(out, s, 30);
	return 30;
}

static inline fa_bool _fa_should_open_quote(char in)
{
	return in == '(' || isspace(in);
}

static inline size_t _fa_finish(char *a_out, enum _fa_state a_state)
{
	char *out = a_out;
	switch (a_state)
	{
		case _fa_state_dot:
			*out++ = '.';
			break;

		case _fa_state_dotdot:
			*out++ = '.';
			*out++ = '.';
			break;

		case _fa_state_dash:
			*out++ = '-';
			break;

		case _fa_state_dashdash:
			out += _fa_write_mdash(out);
			break;

		case _fa_state_tag_start:
		case _fa_state_tag:
			*out++ = '>';
			break;

		case _fa_state_cdata:
			*out++ = ']';
			*out++ = ']';
			*out++ = '>';
			break;

		case _fa_state_attr_squo:
			*out++ = '\'';
			*out++ = '>';
			break;

		case _fa_state_attr_dquo:
			*out++ = '"';
			*out++ = '>';
			break;

		default:
			break;
	}

	*out++ = '\0';

	return out - a_out - 1;
}

char *fast_aleck(fast_aleck_config a_config, char *a_in, size_t a_in_size, size_t *ao_len)
{
	enum _fa_state state = _fa_state_start;

	size_t out_size = (size_t)((float)a_in_size * 1.2);
	char *out_start = malloc(out_size);
	char *out       = out_start;

	char *in_start = a_in;
	char *in       = a_in;

	fa_bool off       = 0;
	fa_bool in_code   = 0;
	fa_bool in_kbd    = 0;
	fa_bool in_pre    = 0;
	fa_bool in_script = 0;

	fa_bool end_tag_slash_detected = 0;

	for (; *in; ++in)
	{
		if (out + 30 >= out_start + out_size)
		{
			char *out_start_old = out_start;
			out_size = (out_size + 30) * 2;
			out_start = realloc(out_start, out_size);
			if (!out_start)
			{
				free(out_start_old);
				return NULL;
			}
			out = out_start + (out - out_start_old);
		}

		switch (state)
		{
			case _fa_state_start:     goto UP_STATE_START;     break;
			case _fa_state_dot:       goto UP_STATE_DOT;       break;
			case _fa_state_dotdot:    goto UP_STATE_DOTDOT;    break;
			case _fa_state_dash:      goto UP_STATE_DASH;      break;
			case _fa_state_dashdash:  goto UP_STATE_DASHDASH;  break;
			case _fa_state_tag:       goto UP_STATE_TAG;       break;
			case _fa_state_tag_start: goto UP_STATE_TAG_START; break;
			case _fa_state_cdata:     goto UP_STATE_CDATA;     break;
			case _fa_state_attr_squo: goto UP_STATE_ATTR_SQUO; break;
			case _fa_state_attr_dquo: goto UP_STATE_ATTR_DQUO; break;
		}

	UP_STATE_START:
		if (*in == '.' && !off)
			state = _fa_state_dot;
		else if (*in == '-' && !off)
			state = _fa_state_dash;
		else if (*in == '\'' && !off)
		{
			if (in == a_in || _fa_should_open_quote(*(in-1)))
				out += _fa_write_single_quote_start(out, a_config.wrap_quotes);
			else
				out += _fa_write_single_quote_end(out, a_config.wrap_quotes);
		}
		else if (*in == '"' && !off)
		{
			if (in == a_in || _fa_should_open_quote(*(in-1)))
				out += _fa_write_double_quote_start(out, a_config.wrap_quotes);
			else
				out += _fa_write_double_quote_end(out, a_config.wrap_quotes);
		}
		else if (*in == '<')
		{
			*out++ = *in;
			state = _fa_state_tag_start;
		}
		else if (*in == '&' && !off && a_config.wrap_amps && (in + 4 < in_start + a_in_size - 1) && 0 == strncmp(in+1, "amp;", 4))
		{
			in += 4;
			out += _fa_write_wrapped_amp(out);
		}
		else
			*out++ = *in;
		continue;

	UP_STATE_DOT:
		if (*in == '.')
			state = _fa_state_dotdot;
		else
		{
			--in;
			*out++ = '.';
			state = _fa_state_start;
		}
		continue;

	UP_STATE_DOTDOT:
		if (*in == '.')
		{
			out += _fa_write_ellipsis(out);
			state = _fa_state_start;
		}
		else
		{
			--in;
			*out++ = '.';
			*out++ = '.';
			state = _fa_state_start;
		}
		continue;

	UP_STATE_DASH:
		if (*in == '-')
			state = _fa_state_dashdash;
		else
		{
			--in;
			*out++ = '-';
			state = _fa_state_start;
		}
		continue;
		
	UP_STATE_DASHDASH:
		if (*in == '-')
		{
			out += _fa_write_mdash(out);
			state = _fa_state_start;
		}
		else
		{
			--in;
			out += _fa_write_mdash(out);
			state = _fa_state_start;
		}
		continue;

	UP_STATE_TAG_START:
		if (*in == '/')
		{
			end_tag_slash_detected = 1;
			*out++ = *in;
			continue;
		}
		else if (!end_tag_slash_detected && 0 == strncmp(in, "![CDATA[", 8))
		{
			in += 8;
			memcpy(out, "![CDATA[", 8);
			out += 8;
			state = _fa_state_cdata;
		}
		else if (0 == strncmp(in, "code", 4) && (isspace(*(in+4)) || *(in+4) == '>')) 
		{
			in += 3;
			memcpy(out, "code", 4);
			out += 4;
			state = _fa_state_tag;

			if (end_tag_slash_detected)
			{
				in_code = 0;
				off     = in_kbd || in_pre || in_script;
			}
			else
			{
				in_code = 1;
				off     = 1;
			}
		}
		else if (0 == strncmp(in, "kbd", 3) && (isspace(*(in+3)) || *(in+3) == '>'))
		{
			in += 2;
			memcpy(out, "kbd", 3);
			out += 3;
			state = _fa_state_tag;

			if (end_tag_slash_detected)
			{
				in_kbd = 0;
				off    = in_code || in_pre || in_script;
			}
			else
			{
				in_kbd = 1;
				off    = 1;
			}
		}
		else if (0 == strncmp(in, "pre", 3) && (isspace(*(in+3)) || *(in+3) == '>'))
		{
			in += 2;
			memcpy(out, "pre", 3);
			out += 3;
			state = _fa_state_tag;

			if (end_tag_slash_detected)
			{
				in_pre = 0;
				off    = in_code || in_kbd || in_script;
			}
			else
			{
				in_pre = 1;
				off    = 1;
			}
		}
		else if (0 == strncmp(in, "script", 6) && (isspace(*(in+6)) || *(in+6) == '>'))
		{
			in += 5;
			memcpy(out, "script", 6);
			out += 6;
			state = _fa_state_tag;

			if (end_tag_slash_detected)
			{
				in_script = 0;
				off       = in_code || in_kbd || in_pre;
			}
			else
			{
				in_script = 1;
				off       = 1;
			}
		}
		else
		{
			*out++ = *in;
			state = _fa_state_tag;
		}
		continue;

	UP_STATE_TAG:
		*out++ = *in;
		if (*in == '>')
		{
			end_tag_slash_detected = 0;
			state = _fa_state_start;
		}
		else if (*in == '\'')
			state = _fa_state_attr_squo;
		else if (*in == '"')
			state = _fa_state_attr_dquo;
		continue;

	UP_STATE_CDATA:
		*out++ = *in;
		if (*in == ']' && (in + 2 < in_start + a_in_size - 1) && 0 == strncmp(in+1, "]>", 2))
		{
			in += 2;
			memcpy(out, "]>", 2);
			out += 2;
			state = _fa_state_start;
		}
		continue;

	UP_STATE_ATTR_SQUO:
		*out++ = *in;
		if (*in == '\'')
			state = _fa_state_tag;
		continue;

	UP_STATE_ATTR_DQUO:
		*out++ = *in;
		if (*in == '"')
			state = _fa_state_tag;
		continue;
	}

	out += _fa_finish(out, state);

	if (ao_len)
		*ao_len = out - out_start;
	return out_start;
}
