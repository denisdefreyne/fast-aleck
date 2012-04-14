#include <ctype.h>
#include <stdlib.h>

typedef char _up_bool;

enum _up_state
{
	_up_state_start,
	_up_state_dot,
	_up_state_dotdot,
	_up_state_dash,
	_up_state_dashdash,
	_up_state_tag,
	_up_state_attr_squo,
	_up_state_attr_dquo
};

static size_t _up_write_ellipsis(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0xA6;

	return 3;
}

static size_t _up_write_mdash(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0x94;

	return 3;
}

static size_t _up_write_single_quote_start(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0x98;

	return 3;
}

static size_t _up_write_single_quote_end(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0x99;

	return 3;
}

static size_t _up_write_double_quote_start(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0x9C;

	return 3;
}

static size_t _up_write_double_quote_end(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0x9D;

	return 3;
}

static _up_bool _up_should_open_quote(char in)
{
	return in == '(' || isspace(in);
}

void ultrapants(char *a_in, char *a_out)
{
	enum _up_state state = _up_state_start;

	char *in  = a_in;
	char *out = a_out;
	for (; *in; ++in)
	{
		switch (state)
		{
			case _up_state_start:     goto UP_STATE_START;     break;
			case _up_state_dot:       goto UP_STATE_DOT;       break;
			case _up_state_dotdot:    goto UP_STATE_DOTDOT;    break;
			case _up_state_dash:      goto UP_STATE_DASH;      break;
			case _up_state_dashdash:  goto UP_STATE_DASHDASH;  break;
			case _up_state_tag:       goto UP_STATE_TAG;       break;
			case _up_state_attr_squo: goto UP_STATE_ATTR_SQUO; break;
			case _up_state_attr_dquo: goto UP_STATE_ATTR_DQUO; break;
		}

	UP_STATE_START:
		if (*in == '.')
			state = _up_state_dot;
		else if (*in == '-')
			state = _up_state_dash;
		else if (*in == '\'')
		{
			if (in > a_in && _up_should_open_quote(*(in-1)))
				out += _up_write_single_quote_start(out);
			else
				out += _up_write_single_quote_end(out);
		}
		else if (*in == '"')
		{
			if (in > a_in && _up_should_open_quote(*(in-1)))
				out += _up_write_double_quote_start(out);
			else
				out += _up_write_double_quote_end(out);
		}
		else if (*in == '<')
		{
			*out++ = *in;
			state = _up_state_tag;
		}
		else
			*out++ = *in;
		continue;

	UP_STATE_DOT:
		if (*in == '.')
			state = _up_state_dotdot;
		else
		{
			--in;
			*out++ = '.';
			state = _up_state_start;
		}
		continue;

	UP_STATE_DOTDOT:
		if (*in == '.')
		{
			out += _up_write_ellipsis(out);
			state = _up_state_start;
		}
		else
		{
			--in;
			*out++ = '.';
			*out++ = '.';
			state = _up_state_start;
		}
		continue;

	UP_STATE_DASH:
		if (*in == '-')
			state = _up_state_dashdash;
		else
		{
			--in;
			*out++ = '-';
			state = _up_state_start;
		}
		continue;
		
	UP_STATE_DASHDASH:
		if (*in == '-')
		{
			out += _up_write_mdash(out);
			state = _up_state_start;
		}
		else
		{
			--in;
			out += _up_write_mdash(out);
			state = _up_state_start;
		}
		continue;

	UP_STATE_TAG:
		*out++ = *in;
		if (*in == '>')
			state = _up_state_start;
		else if (*in == '\'')
			state = _up_state_attr_squo;
		else if (*in == '"')
			state = _up_state_attr_dquo;
		continue;

	UP_STATE_ATTR_SQUO:
		*out++ = *in;
		if (*in == '\'')
			state = _up_state_tag;
		continue;

	UP_STATE_ATTR_DQUO:
		*out++ = *in;
		if (*in == '"')
			state = _up_state_tag;
		continue;
	}
}
