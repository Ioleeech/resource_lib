#ifndef __RT_STRNG_H__
#define __RT_STRNG_H__

#include <stdio.h>

#include "inttypes.h"
#include "platform.h"

#define CURRENT_OFFSET ((uint32_t) -1)

typedef struct _rt_string_t {
    uint32_t length;
    char_t*  ascii;
} rt_string_t;

rt_string_t* get_calculated_string(FILE* stream, uint32_t offset);
rt_string_t* get_terminated_string(FILE* stream, uint32_t offset);
void_t       del_rt_string(rt_string_t* rt_string);

#endif // __RT_STRNG_H__
