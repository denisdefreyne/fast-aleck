#include <stdlib.h>

enum _up_state
{
	_up_state_start,
	_up_state_dot,
	_up_state_dotdot,
	_up_state_dash,
	_up_state_dashdash
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

void ultrapants(char *in, char *out)
{
	enum _up_state state = _up_state_start;

	for (; *in; ++in)
	{
		switch (state)
		{
			case _up_state_start:    goto UP_STATE_START;    break;
			case _up_state_dot:      goto UP_STATE_DOT;      break;
			case _up_state_dotdot:   goto UP_STATE_DOTDOT;   break;
			case _up_state_dash:     goto UP_STATE_DASH;     break;
			case _up_state_dashdash: goto UP_STATE_DASHDASH; break;
		}

		UP_STATE_START:
		if (*in == '.')
			state = _up_state_dot;
		else if (*in == '-')
			state = _up_state_dash;
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
	}
}
