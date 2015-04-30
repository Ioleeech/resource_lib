#include <stdio.h>
#include <stdlib.h>

#include "inttypes.h"
#include "platform.h"

#include "exe_head.h"

static int load_mz_header(FILE* stream, uint32_t offset, mz_header_t* mz_header)
{
    // Check for correct compilation
    if (sizeof(mz_header_t) != MZ_HEADER_SIZE)
        return -1;

    // Read header
    if (fseek(stream, offset, SEEK_SET) != 0)
        return -1;

    if (fread(mz_header, 1, sizeof(mz_header_t), stream) != sizeof(mz_header_t))
        return -1;

    // Check syncword
    if (mz_header->syncword != MZ_HEADER_SYNC)
        return -1;

    return 0;
}

static int load_ne_header(FILE* stream, uint32_t offset, ne_header_t* ne_header)
{
    // Check for correct compilation
    if (sizeof(ne_header_t) != NE_HEADER_SIZE)
        return -1;

    // Read header
    if (fseek(stream, offset, SEEK_SET) != 0)
        return -1;

    if (fread(ne_header, 1, sizeof(ne_header_t), stream) != sizeof(ne_header_t))
        return -1;

    // Check syncword
    if (ne_header->syncword != NE_HEADER_SYNC)
        return -1;

    return 0;
}

static int load_segmented_info(FILE* stream, uint32_t* p_offset, uint16_t* p_syncword)
{
    *p_offset   = 0x0000;
    *p_syncword = 0x0000;

    // Get offset
    uint32_t offset;

    if (fseek(stream, SEGMENTED_HEADER_OFFSET, SEEK_SET) != 0)
        return -1;

    if (fread(&offset, 1, sizeof(uint32_t), stream) != sizeof(uint32_t))
        return -1;

    // Get syncword
    uint16_t syncword;

    if (fseek(stream, offset, SEEK_SET) != 0)
        return -1;

    if (fread(&syncword, 1, sizeof(uint16_t), stream) != sizeof(uint16_t))
        return -1;

    *p_offset   = offset;
    *p_syncword = syncword;

    return 0;
}

mz_header_t* get_mz_header(FILE* stream, uint32_t offset)
{
    mz_header_t* mz_header = (mz_header_t*) malloc(sizeof(mz_header_t));

    if (! mz_header)
        return NULL;

    if (load_mz_header(stream, offset, mz_header) < 0)
    {
        free(mz_header);
        return NULL;
    }

    return mz_header;
}

void_t del_mz_header(mz_header_t* mz_header)
{
    free(mz_header);
}

ne_header_t* get_ne_header(FILE* stream, uint32_t offset)
{
    ne_header_t* ne_header = (ne_header_t*) malloc(sizeof(ne_header_t));

    if (! ne_header)
        return NULL;

    if (load_ne_header(stream, offset, ne_header) < 0)
    {
        free(ne_header);
        return NULL;
    }

    return ne_header;
}

void_t del_ne_header(ne_header_t* ne_header)
{
    free(ne_header);
}

le_header_t* get_le_header(FILE* stream, uint32_t offset)
{
    // TODO
    return NULL;
}

void_t del_le_header(le_header_t* le_header)
{
    free(le_header);
}

lx_header_t* get_lx_header(FILE* stream, uint32_t offset)
{
    // TODO
    return NULL;
}

void_t del_lx_header(lx_header_t* lx_header)
{
    free(lx_header);
}

pe_header_t* get_pe_header(FILE* stream, uint32_t offset)
{
    // TODO
    return NULL;
}

void_t del_pe_header(pe_header_t* pe_header)
{
    free(pe_header);
}

exe_info_t* get_exe_info(FILE* stream, uint32_t offset)
{
    exe_info_t* exe_info = (exe_info_t*) malloc(sizeof(exe_info_t));

    if (! exe_info)
        return NULL;

    // Get MZ header
    exe_info->mz_header = get_mz_header(stream, offset);
    if (! exe_info->mz_header)
    {
        free(exe_info);
        return NULL;
    }

    // Set additional parameters (1 block = 512 bytes, 1 paragraph = 16 bytes)
    exe_info->extra_offset = 0x0200 * exe_info->mz_header->blocks_in_file;

    if ((exe_info->extra_offset)
    &&  (exe_info->mz_header->bytes_in_last_block > 0x0000)
    &&  (exe_info->mz_header->bytes_in_last_block < 0x0200))
        exe_info->extra_offset -= 0x0200 - exe_info->mz_header->bytes_in_last_block;

    exe_info->data_offset  = 0x0010 * exe_info->mz_header->paragraphs_in_header;
    exe_info->is_segmented = (exe_info->mz_header->reloc_table_offset == RELOCATION_TABLE_OFFSET) ? 1 : 0;
    exe_info->ne_header    = NULL;
    exe_info->le_header    = NULL;
    exe_info->lx_header    = NULL;
    exe_info->pe_header    = NULL;

    if (exe_info->is_segmented)
    {
        // Get segmented info
        load_segmented_info(stream, &exe_info->segmented_offset, &exe_info->segmented_syncword);
    }
    else
    {
        exe_info->segmented_offset   = 0x0000;
        exe_info->segmented_syncword = 0x0000;
    }

    if ((! exe_info->segmented_offset) || (! exe_info->segmented_syncword))
        return exe_info;

    switch (exe_info->segmented_syncword)
    {
        case NE_HEADER_SYNC:
            exe_info->ne_header = get_ne_header(stream, exe_info->segmented_offset);
            break;

        case LE_HEADER_SYNC:
            exe_info->le_header = get_le_header(stream, exe_info->segmented_offset);
            break;

        case LX_HEADER_SYNC:
            exe_info->lx_header = get_lx_header(stream, exe_info->segmented_offset);
            break;

        case PE_HEADER_SYNC:
            exe_info->pe_header = get_pe_header(stream, exe_info->segmented_offset);
            break;

        default:
            break;
    }

    return exe_info;
}

void_t del_exe_info(exe_info_t* exe_info)
{
    if (exe_info->ne_header)
        del_ne_header(exe_info->ne_header);

    if (exe_info->le_header)
        del_le_header(exe_info->le_header);

    if (exe_info->lx_header)
        del_lx_header(exe_info->lx_header);

    if (exe_info->pe_header)
        del_pe_header(exe_info->pe_header);

    if (exe_info->mz_header)
        del_mz_header(exe_info->mz_header);

    free(exe_info);
}

resource_table_info_t* get_resource_table_info(FILE* stream, uint32_t offset)
{
    // Check for correct compilation
    if (sizeof(resource_type_t) != RESOURCE_TYPE_SIZE)
        return NULL;

    if (sizeof(resource_info_t) != RESOURCE_INFO_SIZE)
        return NULL;

    // Go to begining of resource table
    if (fseek(stream, offset, SEEK_SET) != 0)
        return NULL;

    // Get alignment shift
    uint16_t alignment_shift;

    if (fread(&alignment_shift, 1, sizeof(uint16_t), stream) != sizeof(uint16_t))
        return NULL;

    uint32_t multiplier = 1 << alignment_shift;

    // Read resource information block
    resource_type_t   resource_type;
    resource_info_t   resource_info;
    resource_entry_t* info_entries    = NULL;
    uint32_t          info_num        = 0;
    uint32_t       i, info_block_size = 0;

    for ( ; ; )
    {
        // Get info about resource type
        if (fread(&resource_type, 1, sizeof(resource_type_t), stream) != sizeof(resource_type_t))
        {
            if (info_entries) free(info_entries);
            return NULL;
        }

        if (resource_type.type_id == 0x0000)
            break;

        // Get info about resources
        for (i = 0; i < resource_type.num_of_resources; i ++)
        {
            info_block_size  += sizeof(resource_entry_t);
            void_t* new_block = realloc(info_entries, info_block_size);

            if (! new_block)
            {
                if (info_entries) free(info_entries);
                return NULL;
            }

            info_entries = (resource_entry_t*) new_block;

            if (fread(&resource_info, 1, sizeof(resource_info_t), stream) != sizeof(resource_info_t))
            {
                if (info_entries) free(info_entries);
                return NULL;
            }

            info_entries[info_num].type_id        = resource_type.type_id;
            info_entries[info_num].resource_id    = resource_info.resource_id;
            info_entries[info_num].flags          = resource_info.flags;
            info_entries[info_num].content_offset = multiplier * resource_info.content_offset;
            info_entries[info_num].content_size   = multiplier * resource_info.content_size;

            info_num ++;
        }
    }

    if (! info_entries)
        return NULL;

    resource_table_info_t* resource_table_info = (resource_table_info_t*) malloc(sizeof(resource_table_info_t));

    if (! resource_table_info)
    {
        free(info_entries);
        return NULL;
    }

    resource_table_info->resource_data_alignment_shift = alignment_shift;
    resource_table_info->info_entries                  = info_entries;
    resource_table_info->info_entries_num              = info_num;

    return resource_table_info;
}

void_t del_resource_table_info(resource_table_info_t* resource_table_info)
{
    free(resource_table_info->info_entries);
    free(resource_table_info);
}

resident_table_info_t* get_resident_table_info(FILE* stream, uint32_t offset)
{
    // Go to begining of resident table
    if (fseek(stream, offset, SEEK_SET) != 0)
        return NULL;

    // Find resident names
    resident_info_t* info_entries    = NULL;
    uint32_t         info_num        = 0;
    uint32_t         info_block_size = 0;

    for ( ; ; )
    {
        // Get text length
        uint8_t length = 0;

        if (fread(&length, 1, 1, stream) != 1)
        {
            if (info_entries) free(info_entries);
            return NULL;
        }

        if (! length)
            break;

        // Skip text data
        if (fseek(stream, length, SEEK_CUR) != 0)
        {
            if (info_entries) free(info_entries);
            return NULL;
        }

        // Get ordinal number
        uint16_t ordinal_number = 0x0000;

        if (fread(&ordinal_number, 1, sizeof(uint16_t), stream) != sizeof(uint16_t))
        {
            if (info_entries) free(info_entries);
            return NULL;
        }

        // Add new entry
        info_block_size  += sizeof(resident_info_t);
        void_t* new_block = realloc(info_entries, info_block_size);

        if (! new_block)
        {
            if (info_entries) free(info_entries);
            return NULL;
        }

        info_entries = (resident_info_t*) new_block;
        info_entries[info_num].name_str_offset = offset;
        info_entries[info_num].ordinal_number  = ordinal_number;

        offset   += length + 3;
        info_num ++;
    }

    if (! info_entries)
        return NULL;

    resident_table_info_t* resident_table_info = (resident_table_info_t*) malloc(sizeof(resident_table_info_t));

    if (! resident_table_info)
    {
        free(info_entries);
        return NULL;
    }

    resident_table_info->info_entries     = info_entries;
    resident_table_info->info_entries_num = info_num;

    return resident_table_info;
}

void_t del_resident_table_info(resident_table_info_t* resident_table_info)
{
    free(resident_table_info->info_entries);
    free(resident_table_info);
}

static inline void_t del_entry_bundles(entries_bundle_t* entry_bundles, uint32_t bundles_num)
{
    if (entry_bundles)
    {
        if (bundles_num)
        {
            uint32_t i; for (i = 0; i < bundles_num; i ++)
            {
                if (entry_bundles[i].segment_entries)
                {
                    free(entry_bundles[i].segment_entries);
                    entry_bundles[i].segment_entries = NULL;
                }
            }
        }

        free(entry_bundles);
    }
}

entry_table_info_t* get_entry_table_info(FILE* stream, uint32_t offset)
{
    // Check for correct compilation
    if (sizeof(fixed_segment_entry_t) != FIX_SEGMENT_ENTRY_SIZE)
        return NULL;

    if (sizeof(moveable_segment_entry_t) != MOV_SEGMENT_ENTRY_SIZE)
        return NULL;

    // Go to begining of resource table
    if (fseek(stream, offset, SEEK_SET) != 0)
        return NULL;

    // Get entry bundles
    entries_bundle_t* entry_bundles      = NULL;
    uint32_t          bundles_num        = 0;
    uint32_t          bundles_block_size = 0;

    for ( ; ; )
    {
        // Get number of entries in current bundle
        uint8_t entries_num = 0;

        if (fread(&entries_num, 1, 1, stream) != 1)
        {
            del_entry_bundles(entry_bundles, bundles_num);
            return NULL;
        }

        if (! entries_num)
            break;

        // Get segment indicator for current bundle
        uint8_t indicator = 0;

        if (fread(&indicator, 1, 1, stream) != 1)
        {
            del_entry_bundles(entry_bundles, bundles_num);
            return NULL;
        }

        // Add new bundle
        bundles_block_size  += sizeof(entries_bundle_t);
        void_t* new_block = realloc(entry_bundles, bundles_block_size);

        if (! new_block)
        {
            del_entry_bundles(entry_bundles, bundles_num);
            return NULL;
        }

        entry_bundles = (entries_bundle_t*) new_block;
        entry_bundles[bundles_num].segment_indicator   = indicator;
        entry_bundles[bundles_num].segment_entries_num = entries_num;

        if (indicator)
        {
            // Add entries
            entry_bundles[bundles_num].segment_entries = (moveable_segment_entry_t*) malloc(sizeof(moveable_segment_entry_t) * entries_num);

            if (! entry_bundles[bundles_num].segment_entries)
            {
                del_entry_bundles(entry_bundles, bundles_num);
                return NULL;
            }

            uint8_t i; for (i = 0; i < entries_num; i ++)
            {
                if (indicator == 0xFF)
                {
                    // Moveable segment
                    moveable_segment_entry_t segment_entry;

                    if (fread(&segment_entry, 1, sizeof(moveable_segment_entry_t), stream) != sizeof(moveable_segment_entry_t))
                    {
                        del_entry_bundles(entry_bundles, bundles_num);
                        return NULL;
                    }

                    entry_bundles[bundles_num].segment_entries[0].flags         = segment_entry.flags;
                    entry_bundles[bundles_num].segment_entries[0].offset        = segment_entry.offset;
                    entry_bundles[bundles_num].segment_entries[0].moveable_word = segment_entry.moveable_word;
                    entry_bundles[bundles_num].segment_entries[0].segment_num   = segment_entry.segment_num;
                }
                else
                {
                    // Fixed segment
                    fixed_segment_entry_t segment_entry;

                    if (fread(&segment_entry, 1, sizeof(fixed_segment_entry_t), stream) != sizeof(fixed_segment_entry_t))
                    {
                        del_entry_bundles(entry_bundles, bundles_num);
                        return NULL;
                    }

                    entry_bundles[bundles_num].segment_entries[0].flags         = segment_entry.flags;
                    entry_bundles[bundles_num].segment_entries[0].offset        = segment_entry.offset;
                    entry_bundles[bundles_num].segment_entries[0].moveable_word = 0x00;
                    entry_bundles[bundles_num].segment_entries[0].segment_num   = 0x00;
                }
            }
        }
        else
            entry_bundles[bundles_num].segment_entries = NULL;

        bundles_num ++;
    }

    if (! entry_bundles)
        return NULL;

    entry_table_info_t* entry_table_info = (entry_table_info_t*) malloc(sizeof(entry_table_info_t));

    if (! entry_table_info)
    {
        del_entry_bundles(entry_bundles, bundles_num);
        return NULL;
    }

    entry_table_info->entry_bundles     = entry_bundles;
    entry_table_info->entry_bundles_num = bundles_num;

    return entry_table_info;
}

void_t del_entry_table_info(entry_table_info_t* entry_table_info)
{
    del_entry_bundles(entry_table_info->entry_bundles, entry_table_info->entry_bundles_num);
    free(entry_table_info);
}
