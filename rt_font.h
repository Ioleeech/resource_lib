#ifndef __RT_FONT_H__
#define __RT_FONT_H__

#include <stdio.h>

#include "inttypes.h"
#include "platform.h"
#include "rt_btmap.h"
#include "rt_strng.h"

// Font directory resource
//
// 0x0000 : 2 bytes : Number of entries
// 0x0002 : N bytes : Entry data
// ...
// 0xXXXX : M bytes : Reserved

// Entry of font directory resource
//
// 0x0000 :   2 bytes : Ordinal number
// 0x0002 : 113 bytes : Font directory info struct
// 0x0073 :   N bytes : Device name (null-terminated string)
// 0xXXXX :   M bytes : Type face name (null-terminated string)

// Font directory info struct
//
// 0x0000 :  2 bytes : Version number (user-defined number of resource data that can be used to identify it)
// 0x0002 :  4 bytes : Size of data in bytes
// 0x0006 : 60 bytes : Copyright (60-character string with the font supplier's copyright information)
// 0x0042 :  2 bytes : Type of font file
//                     Bit 0: 0 = Bitmap font, 1 = Vector font
//                     Bit 1: Always 0
//                     Bit 2: 1 = Font is located in memory at fixed address, 0 = Otherwise
//                     Bits 3-6: Always 0
//                     Bit 7: 1 = Font is realized by device, 0 = Otherwise
//                     Bits 8-15: Reserved (can be 0)
// 0x0044 :  2 bytes : Point size at which this character set looks best
// 0x0046 :  2 bytes : Vertical resolution at which this character set was digitized (in dots per inch)
// 0x0048 :  2 bytes : Horizontal resolution at which this character set was digitized (in dots per inch)
// 0x004A :  2 bytes : Ascent (distance from top of character definition cell to baseline of typographical font)
// 0x004C :  2 bytes : Internal leading
//                     Mount of leading inside bounds set by "Pixel height" member
//                     Accent marks and other diacritical characters can occur in this area
// 0x004E :  2 bytes : External leading
//                     Amount of extra leading that application adds between rows
// 0x0050 :  1 byte  : Italic font mark or zero
// 0x0051 :  1 byte  : Underlined font mark or zero
// 0x0052 :  1 byte  : Strikeout font mark or zero
// 0x0053 :  2 bytes : Font weight (in the range 0 through 1000)
//                     Zero means that default weight is used
//                     300 is light
//                     400 is regular
//                     600 is semi-bold
//                     700 is bold
//                     800 is extra-bold
// 0x0055 :  1 byte  : Font character set
// 0x0056 :  2 bytes : Pixel width
//                     Width of grid on which vector font was digitized
//                     For raster fonts, if it is not equal to zero, it represents width for all characters in bitmap
//                     If member is equal to zero, font has variable-width characters
// 0x0058 :  2 bytes : Pixel height
//                     Height of grid on which vector font was digitized
//                     Height of character bitmap for raster fonts
// 0x005A :  1 byte  : Pitch and family of font
//                     Bit 0: 1 = Font is variable pitch, 0 = Otherwise
//                     Bits 1-3: Reserved
//                     Bits 4-7: Font family:
//                               0x00 = Don't care (don't know)
//                               0x10 = Roman (proportionally spaced fonts with serifs)
//                               0x20 = Swiss (proportionally spaced fonts without serifs)
//                               0x30 = Modern (fixed-pitch fonts)
//                               0x40 = Script
//                               0x50 = Decorative
// 0x005B :  2 bytes : Average width of characters (generally defined as width of letter "x")
//                     This value does not include overhang required for bold or italic characters
// 0x005D :  2 bytes : Maximum width of characters (width of widest character in font)
// 0x005F :  1 byte  : First character code defined in font
// 0x0060 :  1 byte  : Last character code defined in font
// 0x0061 :  1 byte  : Default character to substitute for characters not in font
// 0x0062 :  1 byte  : "Break" character that will be used to define word breaks for text justification
// 0x0063 :  2 bytes : Row width (number of bytes in each row of bitmap)
//                     This value is always even so that rows start on word boundaries
//                     For vector fonts, this member has no meaning
// 0x0065 :  4 bytes : Device name offset (offset in font data to null-terminated string that specifies device name)
//                     For generic font, this value is zero
// 0x0069 :  4 bytes : Type face name offset (offset in font data to null-terminated string that specifies type face name)
// 0x006D :  4 bytes : Reserved

#define FONTDIR_INFO_SIZE 0x71
#define COPYRIGHT_LENGTH  0x3C

#pragma pack(1)
typedef struct _fontdir_info_t {
    uint16_t version;
    uint32_t data_size;
    uint8_t  copyright [COPYRIGHT_LENGTH];
    uint16_t file_type;
    uint16_t font_size;
    uint16_t vertical_dpi;
    uint16_t horizontal_dpi;
    uint16_t ascent;
    uint16_t internal_lead;
    uint16_t external_lead;
    uint8_t  italic;
    uint8_t  underline;
    uint8_t  strikeout;
    uint16_t font_weight;
    uint8_t  character_set;
    uint16_t char_width;
    uint16_t char_height;
    uint8_t  pitch_family;
    uint16_t average_width;
    uint16_t maximum_width;
    uint8_t  first_symbol;
    uint8_t  last_symbol;
    uint8_t  default_symbol;
    uint8_t  break_symbol;
    uint16_t row_width;
    uint32_t dev_name_offset;
    uint32_t type_face_offset;
    uint32_t reserved;
} fontdir_info_t PACKED_STRUCT;
#pragma pack()

typedef struct _fontdir_entry_t {
    uint16_t       ordinal_number;
    fontdir_info_t fontdir_info;
    rt_string_t*   dev_name;
    rt_string_t*   type_face;
} fontdir_entry_t;

typedef struct _rt_fontdir_t {
    uint16_t         fontdir_entries_num;
    fontdir_entry_t* fontdir_entries;
} rt_fontdir_t;

rt_fontdir_t* get_rt_fontdir(FILE* stream, uint32_t offset);
void_t        del_rt_fontdir(rt_fontdir_t* rt_fontdir);

// Font resource
//
// 0x0000 : 118 bytes : Font info struct
// 0x0076 :  30 bytes : Font info extention (it is absent in version 0x0200)
// 0x0076 :   N bytes : Char table:
//   or                 * For bitmap fonts, it is array of entries (each one is 4/6 bytes for Windows 2.x/Windows 3.x)
// 0x0094                 First 2 bytes of each entry is character width.
//                        Last 2/4 bytes is byte offset from beginning of this struct to character bitmap
//                        There is one extra entry at end of table that describes an absolute-space character (100% blank character, not part of charset)
//                      * For fixed-pitch vector fonts, it is array of 2-bytes entries
//                        Each entry specifies offset from start of bitmap to beginning of string of stroke specification units for character
//                        Number of bytes or words to be used for particular character is calculated by subtracting its entry from next one
//                      * For proportionally spaced vector fonts, it is array of 4-bytes entries
//                        Each entry consists from two 2-bytes fields
//                        First field gives starting offset from start of bitmap of character strokes
//                        Second field gives pixel width of character
//                      * Number of entries in table is calculated as ((LastCharCode - FirstCharCode) + 2)
// 0xXXXX :    ...    : Font data
// 0xYYYY :    ...    : Font face name (null-terminated string)
// 0xZZZZ :    ...    : Device name (null-terminated string)

// Font info struct
//
// 0x0000 :  2 bytes : Version (0x0200 or 0x0300)
// 0x0002 :  4 bytes : Size of data in bytes
// 0x0006 : 60 bytes : Copyright (60-character string with copyright information)
// 0x0042 :  2 bytes : Type of font file
//                     Bit 0: 0 = Bitmap font, 1 = Vector font
//                     Bit 1: Always 0
//                     Bit 2: 1 = Font is located in memory at fixed address (see at 0x006D/0x0071), 0 = Otherwise
//                     Bits 3-6: Always 0
//                     Bit 7: 1 = Font is realized by device, 0 = Otherwise
//                     Bits 8-15: Reserved (can be 0)
// 0x0044 :  2 bytes : Point size at which this character set looks best
// 0x0046 :  2 bytes : Vertical resolution at which this character set was digitized (in dots per inch)
// 0x0048 :  2 bytes : Horizontal resolution at which this character set was digitized (in dots per inch)
// 0x004A :  2 bytes : Ascent (distance from top of character definition cell to baseline of typographical font)
// 0x004C :  2 bytes : Internal leading
//                     Mount of leading inside bounds set by "Pixel height" member
//                     Accent marks and other diacritical characters can occur in this area
// 0x004E :  2 bytes : External leading
//                     Amount of extra leading that application adds between rows
// 0x0050 :  1 byte  : Italic font mark (0x01 or 0x00)
// 0x0051 :  1 byte  : Underlined font mark (0x01 or 0x00)
// 0x0052 :  1 byte  : Strikeout font mark (0x01 or 0x00)
// 0x0053 :  2 bytes : Font weight (in the range 0 through 1000)
//                     Zero means that default weight is used
//                     300 is light
//                     400 is regular
//                     600 is semi-bold
//                     700 is bold
//                     800 is extra-bold
// 0x0055 :  1 byte  : Font character set
// 0x0056 :  2 bytes : Pixel width
//                     Width of grid on which vector font was digitized
//                     For raster fonts, if it is not equal to zero, it represents width for all characters in bitmap
//                     If member is equal to zero, font has variable-width characters
// 0x0058 :  2 bytes : Pixel height
//                     Height of grid on which vector font was digitized
//                     Height of character bitmap for raster fonts
// 0x005A :  1 byte  : Pitch and family of font
//                     Bit 0: 1 = Pitch (font width) is variable, 0 = Pitch is fixed
//                     Bits 1-3: Reserved
//                     Bits 4-7: Font family:
//                               0x00 = Don't care (don't know)
//                               0x10 = Roman (proportionally spaced fonts with serifs)
//                               0x20 = Swiss (proportionally spaced fonts without serifs)
//                               0x30 = Modern (fixed-pitch fonts)
//                               0x40 = Script
//                               0x50 = Decorative
// 0x005B :  2 bytes : Average width of characters (generally defined as width of letter "x")
//                     This value does not include overhang required for bold or italic characters
// 0x005D :  2 bytes : Maximum width of characters (width of widest character in font)
// 0x005F :  1 byte  : First character code defined in font
// 0x0060 :  1 byte  : Last character code defined in font
// 0x0061 :  1 byte  : Default character to substitute for characters not in font
// 0x0062 :  1 byte  : "Break" character that will be used to define word breaks for text justification (normally, ASCII space)
// 0x0063 :  2 bytes : Row width (number of bytes in each row of bitmap)
//                     This value is always even so that rows start on word boundaries
//                     For vector fonts, this member has no meaning
// 0x0065 :  4 bytes : Device name offset (offset in font data to null-terminated string that specifies device name)
//                     For generic font, this value is zero
// 0x0069 :  4 bytes : Font face name offset (offset in font data to null-terminated string that specifies type face name)
// 0x006D :  4 bytes : Absolute machine address of font location (must be even)
// 0x0071 :  4 bytes : Offset in file to beginning of font information
//                     If "fixed" bit is set, then it is absolute address of font data (probably in ROM)
//                     For raster fonts, it points to sequence of bytes that make up font's bitmap.
//                     Height of bitmat is height of font, width is sum of widths of characters in font (rounded up to word boundary)
//                     For vector fonts, it points to string of bytes or words (depending on size of digitizing grid)
//                     This string specifies the strokes for each character of font. The dfBitsOffset field must be
//                     This value is always even
// 0x0075 :  1 byte  : Reserved

// Font info extention
//
// 0x0000 :  4 bytes : Glyph flags:
//                     0x0001 = Font is fixed pitch
//                     0x0002 = Font is proportional pitch
//                     0x0004 = Font is an ABC fixed font
//                     0x0008 = Font is an ABC proportional font
//                     0x0010 = Font is one color
//                     0x0020 = Font is 16 color
//                     0x0040 = Font is 256 color
//                     0x0080 = Font is RGB color
// 0x0004 :  2 bytes : Global A space (distance from current position to left edge of bitmap)
// 0x0006 :  2 bytes : Global B space (width of character)
// 0x0008 :  2 bytes : Global C space (distance from right edge of bitmap to new current position)
//                     Increment of character is sum of three spaces (for ABC fixed fonts)
// 0x000A :  4 bytes : Offset to color table for color fonts.
//                     Format of bits is similar to DIB, but without header.
//                     If no color table is needed, this entry is NULL
// 0x000E : 16 bytes : Reserved

#define FONT_INFO_SIZE      0x76
#define FONT_EXTENTION_SIZE 0x1E

#define FONT_VERSION_2    0x0200
#define FONT_VERSION_3    0x0300

#define FONT_TYPE_MASK    0x0001
#define FONT_TYPE_BITMAP  0x0000
#define FONT_TYPE_VECTOR  0x0001

#define FONT_PITCH_MASK     0x01
#define FONT_PITCH_FIXED    0x00
#define FONT_PITCH_VARIABLE 0x01

#define FONT_FAMILY_MASK    0xF0
#define FONT_FAMILY_UNKNOWN 0x00
#define FONT_FAMILY_ROMAN   0x10
#define FONT_FAMILY_SWISS   0x20
#define FONT_FAMILY_MODERN  0x30
#define FONT_FAMILY_SCRIPT  0x40
#define FONT_FAMILY_DECOR   0x50

#pragma pack(1)
typedef struct _font_info_t {
    uint16_t version;
    uint32_t data_size;
    uint8_t  copyright [COPYRIGHT_LENGTH];
    uint16_t file_type;
    uint16_t font_size;
    uint16_t vertical_dpi;
    uint16_t horizontal_dpi;
    uint16_t ascent;
    uint16_t internal_lead;
    uint16_t external_lead;
    uint8_t  italic;
    uint8_t  underline;
    uint8_t  strikeout;
    uint16_t font_weight;
    uint8_t  character_set;
    uint16_t char_width;
    uint16_t char_height;
    uint8_t  pitch_family;
    uint16_t average_width;
    uint16_t maximum_width;
    uint8_t  first_symbol;
    uint8_t  last_symbol;
    uint8_t  default_symbol;
    uint8_t  break_symbol;
    uint16_t row_width;
    uint32_t dev_name_offset;
    uint32_t font_face_offset;
    uint32_t absolute_addr;
    uint32_t data_addr;
    uint8_t  reserved;
} font_info_t PACKED_STRUCT;

typedef struct _font_extention_t {
    uint32_t glyph_flags;
    uint16_t global_a_space;
    uint16_t global_b_space;
    uint16_t global_c_space;
    uint32_t color_table_offset;
    uint8_t  reserved [16];
} font_extention_t PACKED_STRUCT;
#pragma pack()

typedef struct _char_info_t {
    uint16_t width;
    uint32_t offset;
} char_info_t;

typedef struct _rt_font_t {
    font_info_t  font_info;
    uint32_t     data_offset;
    uint32_t     char_table_size;
    char_info_t* char_table;
    rt_string_t* font_face;
    rt_string_t* dev_name;
} rt_font_t;

rt_font_t* get_rt_font(FILE* stream, uint32_t offset);
void_t     del_rt_font(rt_font_t* rt_font);

typedef struct _rt_font_bitmap_t {
    char_t   char_id;
    void_t*  char_data;
    uint32_t data_size;
    uint32_t char_width;
    uint32_t char_height;
    uint32_t bit_count;
    uint32_t line_size;
} rt_font_bitmap_t;

rt_bitmap_data_t* get_rt_font_bitmap_full(FILE* stream, rt_font_t* rt_font);
rt_font_bitmap_t* get_rt_font_bitmap_char(FILE* stream, rt_font_t* rt_font, char_t char_id);
void_t            del_rt_font_bitmap(rt_font_bitmap_t* rt_font_bitmap);

#endif // __RT_FONT_H__
