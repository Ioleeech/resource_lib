#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inttypes.h"
#include "platform.h"

#include "rt_font.h"

static void_t del_fontdir_entries(fontdir_entry_t* entries, uint16_t entries_num)
{
    uint16_t i;

    for (i = 0; i < entries_num; i ++)
    {
        if (entries[i].dev_name)  del_rt_string(entries[i].dev_name);
        if (entries[i].type_face) del_rt_string(entries[i].type_face);
    }

    free(entries);
}

static inline void_t read_char_info_vector_variable(FILE* stream, uint32_t font_offset, char_info_t* char_info)
{
    uint16_t char_offset = 0;
    uint16_t char_width  = 0;

    if ((fread(&char_offset, 1, sizeof(uint16_t), stream) != sizeof(uint16_t))
    ||  (fread(&char_width,  1, sizeof(uint16_t), stream) != sizeof(uint16_t))
    ||  (! char_offset) || (! char_width))
    {
        char_info->offset = 0;
        char_info->width  = 0;
    }
    else
    {
        char_info->offset = char_offset + font_offset;
        char_info->width  = char_width;
    }
}

static inline void_t read_char_info_vector_fixed(FILE* stream, uint32_t font_offset, char_info_t* char_info, uint16_t char_width)
{
    uint16_t char_offset = 0;

    if ((fread(&char_offset, 1, sizeof(uint16_t), stream) != sizeof(uint16_t)) || (! char_offset))
    {
        char_info->offset = 0;
        char_info->width  = 0;
    }
    else
    {
        char_info->offset = char_offset + font_offset;
        char_info->width  = char_width;
    }
}

static inline void_t read_char_info_bitmap_v2(FILE* stream, uint32_t font_offset, char_info_t* char_info)
{
    uint16_t char_width  = 0;
    uint16_t char_offset = 0;

    if ((fread(&char_width,  1, sizeof(uint16_t), stream) != sizeof(uint16_t))
    ||  (fread(&char_offset, 1, sizeof(uint16_t), stream) != sizeof(uint16_t))
    ||  (! char_width) || (! char_offset))
    {
        char_info->offset = 0;
        char_info->width  = 0;
    }
    else
    {
        char_info->offset = char_offset + font_offset;
        char_info->width  = char_width;
    }
}

static inline void_t read_char_info_bitmap_v3(FILE* stream, uint32_t font_offset, char_info_t* char_info)
{
    uint16_t char_width  = 0;
    uint32_t char_offset = 0;

    if ((fread(&char_width,  1, sizeof(uint16_t), stream) != sizeof(uint16_t))
    ||  (fread(&char_offset, 1, sizeof(uint32_t), stream) != sizeof(uint32_t))
    ||  (! char_width) || (! char_offset))
    {
        char_info->offset = 0;
        char_info->width  = 0;
    }
    else
    {
        char_info->offset = char_offset + font_offset;
        char_info->width  = char_width;
    }
}

rt_fontdir_t* get_rt_fontdir(FILE* stream, uint32_t offset)
{
    // Check for correct compilation
    if (sizeof(fontdir_info_t) != FONTDIR_INFO_SIZE)
        return NULL;

    // Go to begining of resource data
    if (fseek(stream, offset, SEEK_SET) != 0)
        return NULL;

    // Get number of entries
    uint16_t i, entries_num = 0;

    if (fread(&entries_num, 1, sizeof(uint16_t), stream) != sizeof(uint16_t))
        return NULL;

    if (entries_num < 1)
        return NULL;

    offset += sizeof(uint16_t);

    // Get entry data
    fontdir_entry_t* entries = (fontdir_entry_t*) malloc(sizeof(fontdir_entry_t) * entries_num);

    if (! entries)
        return NULL;

    for (i = 0; i < entries_num; i ++)
    {
        entries[i].dev_name  = NULL;
        entries[i].type_face = NULL;
    }

    for (i = 0; i < entries_num; i ++)
    {
        if (fseek(stream, offset, SEEK_SET) != 0)
        {
            del_fontdir_entries(entries, entries_num);
            return NULL;
        }

        // Get ordinal number
        if (fread(&entries[i].ordinal_number, 1, sizeof(uint16_t), stream) != sizeof(uint16_t))
        {
            del_fontdir_entries(entries, entries_num);
            return NULL;
        }

        offset += sizeof(uint16_t);

        // Get font directory info struct
        if (fread(&entries[i].fontdir_info, 1, sizeof(fontdir_info_t), stream) != sizeof(fontdir_info_t))
        {
            del_fontdir_entries(entries, entries_num);
            return NULL;
        }

        offset += sizeof(fontdir_info_t);

        // Get device name
        entries[i].dev_name = get_terminated_string(stream, offset);
        offset += (entries[i].dev_name) ? (entries[i].dev_name->length + 1) : 1;

        // Get type face name
        entries[i].type_face = get_terminated_string(stream, offset);
        offset += (entries[i].type_face) ? (entries[i].type_face->length + 1) : 1;
    }

    // Prepare rt_fontdir struct
    rt_fontdir_t* rt_fontdir = (rt_fontdir_t*) malloc(sizeof(rt_fontdir_t));

    if (! rt_fontdir)
    {
        del_fontdir_entries(entries, entries_num);
        return NULL;
    }

    rt_fontdir->fontdir_entries     = entries;
    rt_fontdir->fontdir_entries_num = entries_num;

    return rt_fontdir;
}

void_t del_rt_fontdir(rt_fontdir_t* rt_fontdir)
{
    del_fontdir_entries(rt_fontdir->fontdir_entries, rt_fontdir->fontdir_entries_num);
    free(rt_fontdir);
}

rt_font_t* get_rt_font(FILE* stream, uint32_t offset)
{
    // Check for correct compilation
    if ((sizeof(font_info_t) != FONT_INFO_SIZE) || (sizeof(font_extention_t) != FONT_EXTENTION_SIZE))
        return NULL;

    // Go to begining of resource data
    if (fseek(stream, offset, SEEK_SET) != 0)
        return NULL;

    // Prepare rt_font struct
    rt_font_t* rt_font = (rt_font_t*) malloc(sizeof(rt_font_t));

    if (! rt_font)
        return NULL;

    // Get font info struct
    if (fread(&rt_font->font_info, 1, sizeof(font_info_t), stream) != sizeof(font_info_t))
    {
        free(rt_font);
        return NULL;
    }

    // Get font type
    uint16_t type   = rt_font->font_info.file_type    & FONT_TYPE_MASK;
    uint8_t  pitch  = rt_font->font_info.pitch_family & FONT_PITCH_MASK;
//  uint8_t  family = rt_font->font_info.pitch_family & FONT_FAMILY_MASK;

    // Get data offset
    rt_font->data_offset = offset + rt_font->font_info.data_addr;

    // Get font face name
    if (rt_font->font_info.font_face_offset)
        rt_font->font_face = get_terminated_string(stream, offset + rt_font->font_info.font_face_offset);
    else
        rt_font->font_face = NULL;

    // Get device name
    if (rt_font->font_info.dev_name_offset)
        rt_font->dev_name = get_terminated_string(stream, offset + rt_font->font_info.dev_name_offset);
    else
        rt_font->dev_name = NULL;

    // Get font extention struct
    if (fseek(stream, offset + sizeof(font_info_t), SEEK_SET) != 0)
    {
        if (rt_font->font_face) del_rt_string(rt_font->font_face);
        if (rt_font->dev_name)  del_rt_string(rt_font->dev_name);
        free(rt_font);
        return NULL;
    }

    if (rt_font->font_info.version != FONT_VERSION_2)
    {
        font_extention_t font_extention;

        if (fread(&font_extention, 1, sizeof(font_extention_t), stream) != sizeof(font_extention_t))
        {
            if (rt_font->font_face) del_rt_string(rt_font->font_face);
            if (rt_font->dev_name)  del_rt_string(rt_font->dev_name);
            free(rt_font);
            return NULL;
        }

        //
        // TODO
        //
    }

    // Get character table
    uint32_t i, entries_num = 0;

    if (rt_font->font_info.last_symbol >= rt_font->font_info.first_symbol)
        entries_num = rt_font->font_info.last_symbol - rt_font->font_info.first_symbol + 2;

    if (entries_num)
        rt_font->char_table = (char_info_t*) malloc(sizeof(char_info_t) * entries_num);
    else
        rt_font->char_table = NULL;

    if (rt_font->char_table)
    {
        rt_font->char_table_size = entries_num;

        if (type == FONT_TYPE_VECTOR)
        {
            if (pitch == FONT_PITCH_VARIABLE)
            {
                for (i = 0; i < entries_num; i ++)
                    read_char_info_vector_variable(stream, offset, rt_font->char_table + i);
            }
            else // pitch == FONT_PITCH_FIXED
            {
                for (i = 0; i < entries_num; i ++)
                    read_char_info_vector_fixed(stream, offset, rt_font->char_table + i, rt_font->font_info.char_width);
            }
        }
        else // type == FONT_TYPE_BITMAP
        {
            if (rt_font->font_info.version == FONT_VERSION_2)
            {
                for (i = 0; i < entries_num; i ++)
                    read_char_info_bitmap_v2(stream, offset, rt_font->char_table + i);
            }
            else // rt_font->font_info.version == FONT_VERSION_3
            {
                for (i = 0; i < entries_num; i ++)
                    read_char_info_bitmap_v3(stream, offset, rt_font->char_table + i);
            }
        }
    }
    else
        rt_font->char_table_size = 0;

    return rt_font;
}

void_t del_rt_font(rt_font_t* rt_font)
{
    if (rt_font->font_face)  del_rt_string(rt_font->font_face);
    if (rt_font->dev_name)   del_rt_string(rt_font->dev_name);
    if (rt_font->char_table) free(rt_font->char_table);
    free(rt_font);
}

rt_bitmap_data_t* get_rt_font_bitmap_full(FILE* stream, rt_font_t* rt_font)
{
    if ((! stream) || (! rt_font) || (! rt_font->char_table) || (! rt_font->char_table_size))
        return NULL;

    // Check for font type
    if ((rt_font->font_info.file_type & FONT_TYPE_MASK) != FONT_TYPE_BITMAP)
        return NULL;

    // Generate info
    rt_bitmap_data_t* rt_bitmap_data = (rt_bitmap_data_t*) malloc(sizeof(rt_bitmap_data_t));

    if (! rt_bitmap_data)
        return NULL;

    uint32_t char_bit_count   = (rt_font->font_info.version == FONT_VERSION_2) ? 1 : 0; // TODO
    uint32_t char_line_size   = ((rt_font->font_info.maximum_width * char_bit_count) % 8)
                              ? ((rt_font->font_info.maximum_width * char_bit_count) / 8) + 1
                              : ((rt_font->font_info.maximum_width * char_bit_count) / 8);
    uint32_t char_width       = char_line_size * 8 / char_bit_count;
    uint32_t char_height      = rt_font->font_info.char_height;

    rt_bitmap_data->bit_count = char_bit_count;
    rt_bitmap_data->width     = char_width  * 16;
    rt_bitmap_data->height    = char_height * 16;

    rt_bitmap_data->line_size = calc_bitmap_line_size(rt_bitmap_data->width, rt_bitmap_data->bit_count);
    rt_bitmap_data->size      = rt_bitmap_data->line_size * rt_bitmap_data->height;
    rt_bitmap_data->data      = calloc(rt_bitmap_data->size, 1);

    if (! rt_bitmap_data->data)
    {
        free(rt_bitmap_data);
        return NULL;
    }

    // Get data (symbol by symbol)
    sint_t char_id;

    for (char_id = 0; char_id < 256; char_id ++)
    {
        rt_font_bitmap_t* rt_font_bitmap = get_rt_font_bitmap_char(stream, rt_font, (char_t) char_id);

        if (! rt_font_bitmap)
        {
            del_rt_bitmap_data(rt_bitmap_data);
            return NULL;
        }

        // Insert symbol into bitmap
        uint8_t* src_data  = (uint8_t*) rt_font_bitmap->char_data;

        uint8_t* dst_data  = (uint8_t*) rt_bitmap_data->data;
                 dst_data += (char_id / 16) * char_height * rt_bitmap_data->line_size;
                 dst_data += (char_id % 16) * char_line_size;

        uint32_t char_line;

        for (char_line = 0; char_line < char_height; char_line ++)
        {
            memcpy(dst_data, src_data, rt_font_bitmap->line_size);
            dst_data += rt_bitmap_data->line_size;
            src_data += rt_font_bitmap->line_size;
        }

        del_rt_font_bitmap(rt_font_bitmap);
    }

    return rt_bitmap_data;
}

rt_font_bitmap_t* get_rt_font_bitmap_char(FILE* stream, rt_font_t* rt_font, char_t char_id)
{
    if ((! stream) || (! rt_font) || (! rt_font->char_table) || (! rt_font->char_table_size))
        return NULL;

    // Check for font type
    if ((rt_font->font_info.file_type & FONT_TYPE_MASK) != FONT_TYPE_BITMAP)
        return NULL;

    // Some symbols can be excluded from char table
    char_info_t* char_info = NULL;

    if ((rt_font->font_info.first_symbol > (uint8_t) char_id) || (rt_font->font_info.last_symbol < (uint8_t) char_id))
    {
        uint32_t chars_in_table = rt_font->font_info.last_symbol - rt_font->font_info.first_symbol + 1;

        if (chars_in_table < rt_font->char_table_size)
            char_info = &rt_font->char_table[chars_in_table];
//      else
//          char_info = NULL;
    }
    else
        char_info = &rt_font->char_table[(uint8_t) char_id - rt_font->font_info.first_symbol];

    // Generate info
    rt_font_bitmap_t* rt_font_bitmap = (rt_font_bitmap_t*) malloc(sizeof(rt_font_bitmap_t));

    if (! rt_font_bitmap)
        return NULL;

    rt_font_bitmap->char_id     = char_id;
    rt_font_bitmap->char_height = rt_font->font_info.char_height;
    rt_font_bitmap->char_width  = (char_info) ? char_info->width : rt_font->font_info.average_width;
    rt_font_bitmap->bit_count   = (rt_font->font_info.version == FONT_VERSION_2) ? 1 : 0; // TODO
    rt_font_bitmap->line_size   = ((rt_font_bitmap->char_width * rt_font_bitmap->bit_count) % 8)
                                ? ((rt_font_bitmap->char_width * rt_font_bitmap->bit_count) / 8) + 1
                                : ((rt_font_bitmap->char_width * rt_font_bitmap->bit_count) / 8);
    rt_font_bitmap->data_size   = rt_font_bitmap->line_size * rt_font_bitmap->char_height;
    rt_font_bitmap->char_data   = malloc(rt_font_bitmap->data_size);

    if (! rt_font_bitmap->char_data)
    {
        free(rt_font_bitmap);
        return NULL;
    }

    // Load data
    if (char_info)
    {
        if (fseek(stream, char_info->offset, SEEK_SET) != 0)
        {
            del_rt_font_bitmap(rt_font_bitmap);
            return NULL;
        }

        if (fread(rt_font_bitmap->char_data, 1, rt_font_bitmap->data_size, stream) != (size_t) rt_font_bitmap->data_size)
        {
            del_rt_font_bitmap(rt_font_bitmap);
            return NULL;
        }
    }
    else
    {
        // No real data for this symbol
        memset(rt_font_bitmap->char_data, 0, rt_font_bitmap->data_size);
    }

    return rt_font_bitmap;
}

void_t del_rt_font_bitmap(rt_font_bitmap_t* rt_font_bitmap)
{
    free(rt_font_bitmap->char_data);
    free(rt_font_bitmap);
}
