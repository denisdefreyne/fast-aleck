#ifndef __FAST_ALECK_SLICE_H__
#define __FAST_ALECK_SLICE_H__

#include <stddef.h>
#include <stdio.h>

struct _fa_slice {
	char   *start;
	size_t length;
};
typedef struct _fa_slice fa_slice;

const fa_slice fa_null_slice;

void fa_slice_print(fa_slice slice, FILE *file);

#endif
