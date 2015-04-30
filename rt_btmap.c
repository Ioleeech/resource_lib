#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inttypes.h"
#include "platform.h"

#include "rt_btmap.h"

static void_t read_color_table_core(FILE* stream, rgb_quad_t** p_color_table, uint32_t* p_color_nums, uint16_t bit_count)
{
    uint32_t i, color_nums  = get_colors_num(bit_count);
    rgb_quad_t* color_table = (color_nums) ? (rgb_quad_t*) malloc(sizeof(rgb_quad_t) * color_nums) : NULL;

    if (! color_table)
        return;

    for (i = 0; i < color_nums; i ++)
    {
        rgb_triple_t rgb_triple = { 0, 0, 0 };

        if (fread(&rgb_triple, 1, sizeof(rgb_triple_t), stream) != sizeof(rgb_triple_t))
        {
            free(color_table);
            return;
        }

        color_table[i].blue     = rgb_triple.blue;
        color_table[i].green    = rgb_triple.green;
        color_table[i].red      = rgb_triple.red;
        color_table[i].reserved = 0;
    }

    *p_color_table = color_table;
    *p_color_nums  = color_nums;
}

static void_t read_color_table_info(FILE* stream, rgb_quad_t** p_color_table, uint32_t* p_color_nums, bitmap_info_header_t* info_header)
{
    uint32_t i, color_nums  = (info_header->colors_num_table) ? info_header->colors_num_table : get_colors_num(info_header->bit_count);
    rgb_quad_t* color_table = (color_nums) ? (rgb_quad_t*) malloc(sizeof(rgb_quad_t) * color_nums) : NULL;

    if (! color_table)
        return;

    for (i = 0; i < color_nums; i ++)
    {
        if (fread(color_table + i, 1, sizeof(rgb_quad_t), stream) != sizeof(rgb_quad_t))
        {
            free(color_table);
            return;
        }
    }

    *p_color_table = color_table;
    *p_color_nums  = color_nums;
}

static sint_t save_color_table_core(FILE* stream, rgb_quad_t* color_table, uint32_t color_nums)
{
    uint32_t i;

    for (i = 0; i < color_nums; i ++)
    {
        rgb_triple_t rgb_triple = { color_table[i].blue, color_table[i].green, color_table[i].red };

        if (fwrite(&rgb_triple, 1, sizeof(rgb_triple_t), stream) != sizeof(rgb_triple_t))
            return -1;
    }

    return 0;
}

static sint_t save_color_table_info(FILE* stream, rgb_quad_t* color_table, uint32_t color_nums)
{
    uint32_t i;

    for (i = 0; i < color_nums; i ++)
    {
        if (fwrite(color_table + i, 1, sizeof(rgb_quad_t), stream) != sizeof(rgb_quad_t))
            return -1;
    }

    return 0;
}

static inline uint32_t data_line_size_core(bitmap_core_header_t* core_header)
{
    return calc_bitmap_line_size((uint32_t) core_header->data_width, (uint32_t) core_header->bit_count);
}

static inline uint32_t data_line_size_info(bitmap_info_header_t* info_header)
{
    return calc_bitmap_line_size(info_header->data_width, (uint32_t) info_header->bit_count);
}

static bitmap_core_header_t* generate_core_header(uint32_t compression,
                                                  uint32_t bit_count,
                                                  uint32_t data_width,
                                                  uint32_t data_height)
{
    if (compression != BI_RGB)
        return NULL;

    switch (bit_count)
    {
        case  1:
        case  4:
        case  8:
        case 24:
            break;

        default:
            return NULL;
    }

    bitmap_core_header_t* core_header = (bitmap_core_header_t*) malloc(sizeof(bitmap_core_header_t));
    if (! core_header)
        return NULL;

    core_header->info_size   = BITMAP_CORE_HEADER_SIZE;
    core_header->data_width  = (uint16_t) data_width;
    core_header->data_height = (uint16_t) data_height;
    core_header->planes_num  = 1;
    core_header->bit_count   = (uint16_t) bit_count;

    return core_header;
}

static bitmap_info_header_t* generate_info_header(uint32_t compression,
                                                  uint32_t bit_count,
                                                  uint32_t data_width,
                                                  uint32_t data_height)
{
//  uint32_t colors_num = 0;

    switch (compression)
    {
        case BI_RGB:
        case BI_RLE8:
        case BI_RLE4:
        case BI_BITFIELDS:
        case BI_JPEG:
        case BI_PNG:
        case BI_CMYK:
        case BI_CMYKRLE8:
        case BI_CMYKRLE4:
            break;

        default:
            return NULL;
    }

    switch (bit_count)
    {
        case  1:
        case  4:
        case  8:
        case 24:
//          colors_num = get_colors_num(bit_count);
            break;

        case  0:
        case 16:
        case 32:
            if (BI_RGB == compression)
                return NULL;
            break;

        default:
            return NULL;
    }

    bitmap_info_header_t* info_header = (bitmap_info_header_t*) malloc(sizeof(bitmap_info_header_t));
    if (! info_header)
        return NULL;

    info_header->info_size        = BITMAP_INFO_HEADER_SIZE;
    info_header->data_width       = data_width;
    info_header->data_height      = data_height;
    info_header->planes_num       = 1;
    info_header->bit_count        = (uint16_t) bit_count;
    info_header->compression      = compression;
    info_header->data_size        = data_height * calc_bitmap_line_size(data_width, bit_count);
    info_header->ppm_vertical     = 0;
    info_header->ppm_horizontal   = 0;
    info_header->colors_num_table = 0; // colors_num
    info_header->colors_num_data  = 0; // colors_num

    return info_header;
}

uint32_t calc_bitmap_line_size(uint32_t data_width, uint32_t bit_count)
{
    uint32_t line_size = data_width * bit_count / 8;

    // Line is aligned for 4 bytes
    line_size +=  0x03;
    line_size &= ~0x03;

    return line_size;
}

rt_bitmap_t* get_rt_bitmap(FILE* stream, uint32_t offset)
{
    // Check for correct compilation
    if (( sizeof(bitmap_file_header_t) != BITMAP_FILE_HEADER_SIZE )
    ||  ( sizeof(bitmap_core_header_t) != BITMAP_CORE_HEADER_SIZE )
    ||  ( sizeof(bitmap_info_header_t) != BITMAP_INFO_HEADER_SIZE )
    ||  ( sizeof(bitmap_v4_header_t)   != BITMAP_V4_HEADER_SIZE   )
    ||  ( sizeof(bitmap_v5_header_t)   != BITMAP_V5_HEADER_SIZE   ))
        return NULL;

    // Go to begining of resource data
    if (fseek(stream, offset, SEEK_SET) != 0)
        return NULL;

    // Get file header
    bitmap_file_header_t* file_header = (bitmap_file_header_t*) malloc(sizeof(bitmap_file_header_t));

    if (! file_header)
        return NULL;

    if (fread(file_header, 1, sizeof(bitmap_file_header_t), stream) != sizeof(bitmap_file_header_t))
    {
        free(file_header);
        return NULL;
    }

    if (file_header->syncword != BITMAP_FILE_HEADER_SYNC)
    {
        free(file_header);
        return NULL;
    }

    // Get type of info header
    uint32_t     info_size   = 0;
    info_types_e info_type   = BITMAP_UNKNOWN;
    void_t*      info_header = NULL;

    if (fread(&info_size, 1, sizeof(uint32_t), stream) != sizeof(uint32_t))
    {
        free(file_header);
        return NULL;
    }

    switch (info_size)
    {
        case BITMAP_CORE_HEADER_SIZE:
            info_type   = BITMAP_CORE;
            info_header = malloc(sizeof(bitmap_core_header_t));
            break;

        case BITMAP_INFO_HEADER_SIZE:
            info_type   = BITMAP_INFO;
            info_header = malloc(sizeof(bitmap_info_header_t));
            break;

        case BITMAP_V4_HEADER_SIZE:
            info_type   = BITMAP_V4;
            info_header = malloc(sizeof(bitmap_v4_header_t));
            break;

        case BITMAP_V5_HEADER_SIZE:
            info_type   = BITMAP_V5;
            info_header = malloc(sizeof(bitmap_v5_header_t));
            break;

        default:
            info_type   = BITMAP_UNKNOWN;
            info_header = NULL;
            break;
    }

    if (! info_header)
    {
        free(file_header);
        return NULL;
    }

    // Get info header
    *((uint32_t*) info_header) = info_size;
    info_size -= sizeof(uint32_t);

    if (fread(info_header + sizeof(uint32_t), 1, info_size, stream) != info_size)
    {
        free(info_header);
        free(file_header);
        return NULL;
    }

    // Get color table
    rgb_quad_t* color_table = NULL;
    uint32_t    color_nums  = 0;

    if (BITMAP_CORE == info_type)
        read_color_table_core(stream, &color_table, &color_nums, ((bitmap_core_header_t*) info_header)->bit_count);
    else
        read_color_table_info(stream, &color_table, &color_nums, (bitmap_info_header_t*) info_header);

    // Prepare rt_bitmap struct
    rt_bitmap_t* rt_bitmap = (rt_bitmap_t*) malloc(sizeof(rt_bitmap_t));

    if (! rt_bitmap)
    {
        if (color_table)
            free(color_table);
        free(info_header);
        free(file_header);
        return NULL;
    }

    rt_bitmap->file_header = file_header;
    rt_bitmap->info_header = info_header;
    rt_bitmap->info_type   = info_type;
    rt_bitmap->color_table = color_table;
    rt_bitmap->color_nums  = color_nums;
    rt_bitmap->data_offset = offset + file_header->data_offset;
    rt_bitmap->data_line   = (BITMAP_CORE == info_type)
                           ? data_line_size_core((bitmap_core_header_t*) info_header)
                           : data_line_size_info((bitmap_info_header_t*) info_header);
    rt_bitmap->data_size   = (BITMAP_CORE == info_type)
                           ? ((bitmap_core_header_t*) info_header)->data_height * rt_bitmap->data_line
                           : ((bitmap_info_header_t*) info_header)->data_size;
    return rt_bitmap;
}

rt_bitmap_t* gen_rt_bitmap(info_types_e info_type,
                           uint32_t     compression,
                           uint32_t     bit_count,
                           uint32_t     data_width,
                           uint32_t     data_height)
{
    // Check for correct compilation
    if (( sizeof(bitmap_file_header_t) != BITMAP_FILE_HEADER_SIZE )
    ||  ( sizeof(bitmap_core_header_t) != BITMAP_CORE_HEADER_SIZE )
    ||  ( sizeof(bitmap_info_header_t) != BITMAP_INFO_HEADER_SIZE )
    ||  ( sizeof(bitmap_v4_header_t)   != BITMAP_V4_HEADER_SIZE   )
    ||  ( sizeof(bitmap_v5_header_t)   != BITMAP_V5_HEADER_SIZE   ))
        return NULL;

    // Generate file header
    bitmap_file_header_t* file_header = (bitmap_file_header_t*) malloc(sizeof(bitmap_file_header_t));

    if (! file_header)
        return NULL;

    file_header->syncword    = BITMAP_FILE_HEADER_SYNC;
    file_header->file_size   = BITMAP_FILE_HEADER_SIZE;
    file_header->reserved    = 0;
    file_header->data_offset = BITMAP_FILE_HEADER_SIZE;

    // Generate info header
    void_t*  info_header = NULL;
    uint32_t info_size   = 0;

    switch (info_type)
    {
        case BITMAP_CORE:
            info_header = generate_core_header(compression, bit_count, data_width, data_height);
            info_size   = BITMAP_CORE_HEADER_SIZE;
            break;

        case BITMAP_INFO:
            info_header = generate_info_header(compression, bit_count, data_width, data_height);
            info_size   = BITMAP_INFO_HEADER_SIZE;
            break;

        case BITMAP_V4:
        case BITMAP_V5:
            //
            // TODO
            //

//      case BITMAP_UNKNOWN:
        default:
            info_header = NULL;
            info_size   = 0;
            break;
    }

    if (! info_header)
    {
        free(file_header);
        return NULL;
    }

    file_header->file_size   += info_size;
    file_header->data_offset += info_size;

    // Generate color table
    uint32_t    color_nums  = get_colors_num(bit_count);
    rgb_quad_t* color_table = (color_nums) ? get_color_table_quad(bit_count) : NULL;

    if ((color_table) && (color_nums))
    {
        uint32_t table_size = ((BITMAP_CORE == info_type) ? sizeof(rgb_triple_t) : sizeof(rgb_quad_t)) * color_nums;

        file_header->file_size   += table_size;
        file_header->data_offset += table_size;
    }

    // Prepare rt_bitmap struct
    rt_bitmap_t* rt_bitmap = (rt_bitmap_t*) malloc(sizeof(rt_bitmap_t));

    if (! rt_bitmap)
    {
        if (color_table)
            free(color_table);
        free(info_header);
        free(file_header);
        return NULL;
    }

    rt_bitmap->file_header = file_header;
    rt_bitmap->info_header = info_header;
    rt_bitmap->info_type   = info_type;
    rt_bitmap->color_table = color_table;
    rt_bitmap->color_nums  = color_nums;
    rt_bitmap->data_offset = file_header->data_offset;
    rt_bitmap->data_line   = (BITMAP_CORE == info_type)
                           ? data_line_size_core((bitmap_core_header_t*) info_header)
                           : data_line_size_info((bitmap_info_header_t*) info_header);
    rt_bitmap->data_size   = data_height * rt_bitmap->data_line;

    // Do not forget about data
    file_header->file_size += rt_bitmap->data_size;

    return rt_bitmap;
}

sint_t put_rt_bitmap(FILE* stream, uint32_t offset, rt_bitmap_t* rt_bitmap)
{
    sint_t ret = -1;

    if ((! stream) || (! rt_bitmap))
        return -1;

    // Go to resource placement
    if (fseek(stream, offset, SEEK_SET) != 0)
        return -1;

    // Save file header
    if (fwrite(rt_bitmap->file_header, 1, sizeof(bitmap_file_header_t), stream) != sizeof(bitmap_file_header_t))
        return -1;

    switch (rt_bitmap->info_type)
    {
        case BITMAP_CORE:
            ret = (fwrite(rt_bitmap->info_header, 1, sizeof(bitmap_core_header_t), stream) != sizeof(bitmap_core_header_t)) ? -1 : 0;
            break;

        case BITMAP_INFO:
            ret = (fwrite(rt_bitmap->info_header, 1, sizeof(bitmap_info_header_t), stream) != sizeof(bitmap_info_header_t)) ? -1 : 0;
            break;

        case BITMAP_V4:
        case BITMAP_V5:
            //
            // TODO
            //

//      case BITMAP_UNKNOWN:
        default:
            break;
    }

    if (ret < 0)
        return ret;

    // Save color table
    if ((rt_bitmap->color_table) && (rt_bitmap->color_nums))
    {
        if (BITMAP_CORE == rt_bitmap->info_type)
            ret = save_color_table_core(stream, rt_bitmap->color_table, rt_bitmap->color_nums);
        else
            ret = save_color_table_info(stream, rt_bitmap->color_table, rt_bitmap->color_nums);
    }

    if (ret < 0)
        return ret;

    return 0;
}

void_t del_rt_bitmap(rt_bitmap_t* rt_bitmap)
{
    if (rt_bitmap->color_table)
        free(rt_bitmap->color_table);
    free(rt_bitmap->info_header);
    free(rt_bitmap->file_header);
    free(rt_bitmap);
}

rt_bitmap_data_t* get_rt_bitmap_data(FILE* stream, rt_bitmap_t* rt_bitmap)
{
    if ((! stream) || (! rt_bitmap) || (! rt_bitmap->data_offset) || (! rt_bitmap->data_size))
        return NULL;

    rt_bitmap_data_t* rt_bitmap_data = (rt_bitmap_data_t*) malloc(sizeof(rt_bitmap_data_t));

    if (! rt_bitmap_data)
        return NULL;

    if (rt_bitmap->info_type == BITMAP_CORE)
    {
        bitmap_core_header_t* core_header = (bitmap_core_header_t*) rt_bitmap->info_header;

        rt_bitmap_data->bit_count = core_header->bit_count;
        rt_bitmap_data->width     = core_header->data_width;
        rt_bitmap_data->height    = core_header->data_height;
    }
    else
    {
        bitmap_info_header_t* info_header = (bitmap_info_header_t*) rt_bitmap->info_header;

        rt_bitmap_data->bit_count = info_header->bit_count;
        rt_bitmap_data->width     = info_header->data_width;
        rt_bitmap_data->height    = info_header->data_height;
    }

    rt_bitmap_data->line_size = rt_bitmap->data_line;
    rt_bitmap_data->size      = rt_bitmap->data_size;
    rt_bitmap_data->data      = malloc(rt_bitmap->data_size);

    if (! rt_bitmap_data->data)
    {
        free(rt_bitmap_data);
        return NULL;
    }

    if (fseek(stream, rt_bitmap->data_offset, SEEK_SET) != 0)
    {
        del_rt_bitmap_data(rt_bitmap_data);
        return NULL;
    }

    uint8_t* line  = (uint8_t*) rt_bitmap_data->data;
             line += rt_bitmap_data->size;
             line -= rt_bitmap_data->line_size;

    uint32_t i;
    for (i = 0; i < rt_bitmap_data->height; i ++, line -= rt_bitmap_data->line_size)
    {
        if (fread(line, 1, rt_bitmap_data->line_size, stream) != (size_t) rt_bitmap_data->line_size)
        {
            del_rt_bitmap_data(rt_bitmap_data);
            return NULL;
        }
    }

    return rt_bitmap_data;
}

sint_t put_rt_bitmap_data(FILE* stream, uint32_t offset, rt_bitmap_data_t* rt_bitmap_data)
{
    if ((! stream) || (! rt_bitmap_data))
        return -1;

    if (fseek(stream, offset, SEEK_SET) != 0)
        return -1;

    uint8_t* line  = (uint8_t*) rt_bitmap_data->data;
             line += rt_bitmap_data->size;
             line -= rt_bitmap_data->line_size;

    uint32_t i;
    for (i = 0; i < rt_bitmap_data->height; i ++, line -= rt_bitmap_data->line_size)
    {
        if (fwrite(line, 1, rt_bitmap_data->line_size, stream) != (size_t) rt_bitmap_data->line_size)
            return -1;
    }

    return 0;
}

void_t del_rt_bitmap_data(rt_bitmap_data_t* rt_bitmap_data)
{
    free(rt_bitmap_data->data);
    free(rt_bitmap_data);
}
