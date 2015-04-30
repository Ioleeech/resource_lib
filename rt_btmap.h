#ifndef __RT_BTMAP_H__
#define __RT_BTMAP_H__

#include <stdio.h>

#include "inttypes.h"
#include "platform.h"
#include "colortbl.h"

// Bitmap file header
//
// 0x0000 : "BM"    : 0x42 0x4D
// 0x0002 : 4 bytes : File size in bytes
// 0x0006 : 4 bytes : Reserved
// 0x000A : 4 bytes : Offset to pixel array

// Bitmap info header (BITMAPCOREHEADER)
//
// 0x0000 : 4 bytes : Header size
// 0x0004 : 2 bytes : Image width
// 0x0006 : 2 bytes : Image height
// 0x0008 : 2 bytes : Number of planes (always 1)
// 0x000A : 2 bytes : Bits per pixel   (1, 4, 8 or 24)

// Bitmap info header (BITMAPINFOHEADER)
//
// 0x0000 : 4 bytes : Header size
// 0x0004 : 4 bytes : Image width
// 0x0008 : 4 bytes : Image height
// 0x000C : 2 bytes : Number of planes (always 1)
// 0x000E : 2 bytes : Bits per pixel   (0, 1, 4, 8, 16, 24 or 32)
// 0x0010 : 4 bytes : Compression type (see compression_e enum)
// 0x0014 : 4 bytes : Image size in bytes
// 0x0018 : 4 bytes : Horizontal resolution (in pixels per meter)
// 0x001C : 4 bytes : Vertical resolution (in pixels per meter)
// 0x0020 : 4 bytes : Number of colors in table
// 0x0024 : 4 bytes : Number of colors in image

// Bitmap info header (BITMAPV4HEADER)
//
// 0x0000 :  4 bytes : Header size
// 0x0004 :  4 bytes : Image width
// 0x0008 :  4 bytes : Image height
// 0x000C :  2 bytes : Number of planes (always 1)
// 0x000E :  2 bytes : Bits per pixel   (0, 1, 4, 8, 16, 24 or 32)
// 0x0010 :  4 bytes : Compression type (see compression_e enum)
// 0x0014 :  4 bytes : Image size in bytes
// 0x0018 :  4 bytes : Horizontal resolution (in pixels per meter)
// 0x001C :  4 bytes : Vertical resolution (in pixels per meter)
// 0x0020 :  4 bytes : Number of colors in table
// 0x0024 :  4 bytes : Number of colors in image
// 0x0028 :  4 bytes : Red color mask   (for BI_BITFIELDS compression only)
// 0x002C :  4 bytes : Green color mask (for BI_BITFIELDS compression only)
// 0x0030 :  4 bytes : Blue color mask  (for BI_BITFIELDS compression only)
// 0x0034 :  4 bytes : Alpha channel mask
// 0x0038 :  4 bytes : Color space type
// 0x003C : 12 bytes : Red color endpoint   (4 bytes for each: X, Y, Z)
// 0x0048 : 12 bytes : Green color endpoint (4 bytes for each: X, Y, Z)
// 0x0054 : 12 bytes : Blue color endpoint  (4 bytes for each: X, Y, Z)
// 0x0060 :  4 bytes : Gamma (tone response curve) for red color   (in unsigned fixed 16.16 format)
// 0x0064 :  4 bytes : Gamma (tone response curve) for green color (in unsigned fixed 16.16 format)
// 0x0068 :  4 bytes : Gamma (tone response curve) for blue color  (in unsigned fixed 16.16 format)

// Bitmap info header (BITMAPV5HEADER)
//
// 0x0000 :  4 bytes : Header size
// 0x0004 :  4 bytes : Image width
// 0x0008 :  4 bytes : Image height
// 0x000C :  2 bytes : Number of planes (always 1)
// 0x000E :  2 bytes : Bits per pixel   (0, 1, 4, 8, 16, 24 or 32)
// 0x0010 :  4 bytes : Compression type (see compression_e enum)
// 0x0014 :  4 bytes : Image size in bytes
// 0x0018 :  4 bytes : Horizontal resolution (in pixels per meter)
// 0x001C :  4 bytes : Vertical resolution (in pixels per meter)
// 0x0020 :  4 bytes : Number of colors in table
// 0x0024 :  4 bytes : Number of colors in image
// 0x0028 :  4 bytes : Red color mask   (for BI_BITFIELDS compression only)
// 0x002C :  4 bytes : Green color mask (for BI_BITFIELDS compression only)
// 0x0030 :  4 bytes : Blue color mask  (for BI_BITFIELDS compression only)
// 0x0034 :  4 bytes : Alpha channel mask
// 0x0038 :  4 bytes : Color space type
// 0x003C : 12 bytes : Red color endpoint   (4 bytes for each: X, Y, Z)
// 0x0048 : 12 bytes : Green color endpoint (4 bytes for each: X, Y, Z)
// 0x0054 : 12 bytes : Blue color endpoint  (4 bytes for each: X, Y, Z)
// 0x0060 :  4 bytes : Gamma (tone response curve) for red color   (in unsigned fixed 16.16 format)
// 0x0064 :  4 bytes : Gamma (tone response curve) for green color (in unsigned fixed 16.16 format)
// 0x0068 :  4 bytes : Gamma (tone response curve) for blue color  (in unsigned fixed 16.16 format)
// 0x006C :  4 bytes : Rendering intent
// 0x0070 :  4 bytes : Offset from beginning of info header to profile data (in bytes)
// 0x0074 :  4 bytes : Size of profile data (in bytes)
// 0x0078 :  4 bytes : Reserved

#define BITMAP_FILE_HEADER_SIZE 0x0E
#define BITMAP_FILE_HEADER_SYNC 0x4D42

#define BITMAP_CORE_HEADER_SIZE 0x0C
#define BITMAP_INFO_HEADER_SIZE 0x28
#define BITMAP_V4_HEADER_SIZE   0x6C
#define BITMAP_V5_HEADER_SIZE   0x7C

typedef enum _info_types_e {
    BITMAP_CORE,
    BITMAP_INFO,
    BITMAP_V4,
    BITMAP_V5,
    BITMAP_UNKNOWN
} info_types_e;

typedef enum _compression_e {
  BI_RGB       = 0x0000,
  BI_RLE8      = 0x0001,
  BI_RLE4      = 0x0002,
  BI_BITFIELDS = 0x0003,
  BI_JPEG      = 0x0004,
  BI_PNG       = 0x0005,
  BI_CMYK      = 0x000B,
  BI_CMYKRLE8  = 0x000C,
  BI_CMYKRLE4  = 0x000D
} compression_e;

#pragma pack(1)
typedef struct _endpoint_t {
    uint32_t x;
    uint32_t y;
    uint32_t z;
} endpoint_t PACKED_STRUCT;

typedef struct _bitmap_file_header_t {
    uint16_t syncword;
    uint32_t file_size;
    uint32_t reserved;
    uint32_t data_offset;
} bitmap_file_header_t PACKED_STRUCT;

typedef struct _bitmap_core_header_t {
    uint32_t info_size;
    uint16_t data_width;
    uint16_t data_height;
    uint16_t planes_num;
    uint16_t bit_count;
} bitmap_core_header_t PACKED_STRUCT;

typedef struct _bitmap_info_header_t {
    uint32_t info_size;
    uint32_t data_width;
    uint32_t data_height;
    uint16_t planes_num;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t data_size;
    uint32_t ppm_vertical;
    uint32_t ppm_horizontal;
    uint32_t colors_num_table;
    uint32_t colors_num_data;
} bitmap_info_header_t PACKED_STRUCT;

typedef struct _bitmap_v4_header_t {
    uint32_t   info_size;
    uint32_t   data_width;
    uint32_t   data_height;
    uint16_t   planes_num;
    uint16_t   bit_count;
    uint32_t   compression;
    uint32_t   data_size;
    uint32_t   ppm_vertical;
    uint32_t   ppm_horizontal;
    uint32_t   colors_num_table;
    uint32_t   colors_num_data;
    uint32_t   mask_red;
    uint32_t   mask_green;
    uint32_t   mask_blue;
    uint32_t   mask_alpha;
    uint32_t   color_space;
    endpoint_t endpoint_red;
    endpoint_t endpoint_green;
    endpoint_t endpoint_blue;
    uint32_t   gamma_red;
    uint32_t   gamma_green;
    uint32_t   gamma_blue;
} bitmap_v4_header_t PACKED_STRUCT;

typedef struct _bitmap_v5_header_t {
    uint32_t   info_size;
    uint32_t   data_width;
    uint32_t   data_height;
    uint16_t   planes_num;
    uint16_t   bit_count;
    uint32_t   compression;
    uint32_t   data_size;
    uint32_t   ppm_vertical;
    uint32_t   ppm_horizontal;
    uint32_t   colors_num_table;
    uint32_t   colors_num_data;
    uint32_t   mask_red;
    uint32_t   mask_green;
    uint32_t   mask_blue;
    uint32_t   mask_alpha;
    uint32_t   color_space;
    endpoint_t endpoint_red;
    endpoint_t endpoint_green;
    endpoint_t endpoint_blue;
    uint32_t   gamma_red;
    uint32_t   gamma_green;
    uint32_t   gamma_blue;
    uint32_t   intent;
    uint32_t   profile_offset;
    uint32_t   profile_size;
    uint32_t   reserved;
} bitmap_v5_header_t PACKED_STRUCT;
#pragma pack()

uint32_t calc_bitmap_line_size(uint32_t data_width, uint32_t bit_count);

typedef struct _rt_bitmap_t {
    bitmap_file_header_t* file_header;
    info_types_e          info_type;
    void_t*               info_header;
    rgb_quad_t*           color_table;
    uint32_t              color_nums;
    uint32_t              data_offset;
    uint32_t              data_line;
    uint32_t              data_size;
} rt_bitmap_t;

rt_bitmap_t* get_rt_bitmap(FILE* stream, uint32_t offset);
rt_bitmap_t* gen_rt_bitmap(info_types_e info_type,
                           uint32_t     compression,
                           uint32_t     bit_count,
                           uint32_t     data_width,
                           uint32_t     data_height);
sint_t       put_rt_bitmap(FILE* stream, uint32_t offset, rt_bitmap_t* rt_bitmap);
void_t       del_rt_bitmap(rt_bitmap_t* rt_bitmap);

typedef struct _rt_bitmap_data_t {
    void_t*  data;
    uint32_t size;
    uint32_t bit_count;
    uint32_t width;
    uint32_t height;
    uint32_t line_size;
} rt_bitmap_data_t;

rt_bitmap_data_t* get_rt_bitmap_data(FILE* stream, rt_bitmap_t* rt_bitmap);
sint_t            put_rt_bitmap_data(FILE* stream, uint32_t offset, rt_bitmap_data_t* rt_bitmap_data);
void_t            del_rt_bitmap_data(rt_bitmap_data_t* rt_bitmap_data);

#endif // __RT_FONT_H__
