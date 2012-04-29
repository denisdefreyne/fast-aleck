#ifndef __FAST_ALECK_FAST_ALECK_H__
#define __FAST_ALECK_FAST_ALECK_H__

typedef char fa_bool;

typedef struct _fast_aleck_config {
	fa_bool wrap_amps;
} fast_aleck_config;

char *fast_aleck(fast_aleck_config config, char *in, size_t in_size);

#endif
