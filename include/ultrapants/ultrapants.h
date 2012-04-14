#ifndef __ULTRAPANTS_ULTRAPANTS_H__
#define __ULTRAPANTS_ULTRAPANTS_H__

typedef char up_bool;

typedef struct _ultrapants_config {
	up_bool wrap_amps;
} ultrapants_config;

char *ultrapants(ultrapants_config config, char *in, size_t in_size);

#endif
