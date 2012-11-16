#ifndef __FAST_ALECK_SLICE_H__
#define __FAST_ALECK_SLICE_H__

#include <stddef.h>

struct _fa_slice {
	char   *start;
	size_t length;
};
typedef struct _fa_slice fa_slice;

#endif
