#ifndef __COLORTBL_H__
#define __COLORTBL_H__

#include <stdio.h>

#include "inttypes.h"
#include "platform.h"

#pragma pack(1)
typedef struct _rgb_triple_t {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} rgb_triple_t PACKED_STRUCT;

typedef struct _rgb_quad_t {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;
} rgb_quad_t PACKED_STRUCT;
#pragma pack()

#define COLORS_IN_1_BIT_TABLE 2
#define COLORS_IN_4_BIT_TABLE 16
#define COLORS_IN_8_BIT_TABLE 256

extern const rgb_triple_t color_table_1_bit [COLORS_IN_1_BIT_TABLE];
extern const rgb_triple_t color_table_4_bit [COLORS_IN_4_BIT_TABLE];
extern const rgb_triple_t color_table_8_bit [COLORS_IN_8_BIT_TABLE];

uint32_t      get_colors_num         (uint32_t bit_count);
rgb_triple_t* get_color_table_triple (uint32_t bit_count);
rgb_quad_t*   get_color_table_quad   (uint32_t bit_count);

#endif // __COLORTBL_H__
