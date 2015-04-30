#ifndef __EXE_HEAD_H__
#define __EXE_HEAD_H__

#include <stdio.h>

#include "inttypes.h"
#include "platform.h"

// MZ header
//
// 0x0000 : "MZ"    : 0x4D 0x5A
// 0x0002 : 2 bytes : Number of bytes in program last block that are actually used
// 0x0004 : 2 bytes : File length in blocks (1 block = 512 bytes)
// 0x0006 : 2 bytes : Number of items in relocation table (each item is 4 bytes: offset + segment)
// 0x0008 : 2 bytes : Number of paragraphs in header (1 paragraph = 16 bytes)
// 0x000A : 2 bytes : Additional memory in paragraphs (min of required)
// 0x000C : 2 bytes : Additional memory in paragraphs (max of required)
// 0x000E : 2 bytes : Relative value of stack segment (it is used to initialize SS register)
// 0x0010 : 2 bytes : Initial value of SP register
// 0x0012 : 2 bytes : Checksum (0x0000 or 16-bit sum of all words in file should be zero)
// 0x0014 : 2 bytes : Initial value of IP register
// 0x0016 : 2 bytes : Initial value of CS register
// 0x0018 : 2 bytes : Offset to relocation table
// 0x001A : 2 bytes : Overlay number (0x0000 meaning that it is main program)

#define MZ_HEADER_SIZE 0x1C
#define MZ_HEADER_SYNC 0x5A4D

#pragma pack(1)
typedef struct _mz_header_t {
    uint16_t syncword;
    uint16_t bytes_in_last_block;
    uint16_t blocks_in_file;
    uint16_t items_in_reloc_table;
    uint16_t paragraphs_in_header;
    uint16_t add_paragraphs_min;
    uint16_t add_paragraphs_max;
    uint16_t ss_reg_initval;
    uint16_t sp_reg_initval;
    uint16_t checksum;
    uint16_t ip_reg_initval;
    uint16_t cs_reg_initval;
    uint16_t reloc_table_offset;
    uint16_t overlay_num;
} mz_header_t PACKED_STRUCT;
#pragma pack()

mz_header_t* get_mz_header(FILE* stream, uint32_t offset);
void_t       del_mz_header(mz_header_t* mz_header);

// NE header
//
// 0x0000 : "NE"    : 0x4E 0x45
// 0x0002 : 1 byte  : Version number of linker
// 0x0003 : 1 byte  : Revision number of linker
// 0x0004 : 2 bytes : Entry table offset (relative to beginning of NE header)
// 0x0006 : 2 bytes : Entry table size (in bytes)
// 0x0008 : 4 bytes : 32-bit CRC (This 4 bytes are taken as 0x00 during calculation)
// 0x000C : 2 bytes : Flags (0x0000 = NOAUTODATA, 0x0001 = SINGLEDATA, 0x0002 = MULTIPLEDATA, 0x2000 = LINKERROR, 0x8000 = LIBMODULE)
// 0x000E : 2 bytes : Segment number of automatic data segment
// 0x0010 : 2 bytes : Initial size of dynamic heap (in bytes)
// 0x0012 : 2 bytes : Initial size of stack (in bytes)
// 0x0014 : 4 bytes : Segment number:offset of CS:IP
// 0x0018 : 4 bytes : Segment number:offset of SS:SP
// 0x001C : 2 bytes : Number of entries in segment table
// 0x001E : 2 bytes : Number of entries in module reference table
// 0x0020 : 2 bytes : Non-resident name table size (in bytes)
// 0x0022 : 2 bytes : Segment table offset (relative to beginning of NE header)
// 0x0024 : 2 bytes : Resource table offset (relative to beginning of NE header)
// 0x0026 : 2 bytes : Resident name table offset (relative to beginning of NE header)
// 0x0028 : 2 bytes : Module reference table offset (relative to beginning of NE header)
// 0x002A : 2 bytes : Imported names table offset (relative to beginning of NE header)
// 0x002C : 4 bytes : Non-resident name table offset (relative to beginning of NE header)
// 0x0030 : 2 bytes : Number of movable entries in entry table
// 0x0032 : 2 bytes : Logical sector alignment shift count (log2 of segment sector size)
// 0x0034 : 2 bytes : Number of resource entries
// 0x0036 : 1 byte  : Executable type (0x02 = WINDOWS)
// 0x0037 :         : Reserved
// ...    :         : Reserved
// 0x003F :         : Reserved

#define NE_HEADER_SIZE 0x40
#define NE_HEADER_SYNC 0x454E

#pragma pack(1)
typedef struct _ne_header_t {
    uint16_t syncword;
    uint8_t  linker_version;
    uint8_t  linker_revision;
    uint16_t entry_table_offset;
    uint16_t entry_table_size;
    uint32_t checksum;
    uint16_t flags;
    uint16_t segment_number;
    uint16_t heap_init_size;
    uint16_t stack_init_size;
    uint32_t cs_ip_offset;
    uint32_t ss_sp_offset;
    uint16_t entries_in_segment_table;
    uint16_t entries_in_modref_table;
    uint16_t nonresident_table_size;
    uint16_t segment_table_offset;
    uint16_t resource_table_offset;
    uint16_t resident_table_offset;
    uint16_t modref_table_offset;
    uint16_t imported_table_offset;
    uint32_t nonresident_table_offset;
    uint16_t movable_entries_in_table;
    uint16_t logical_sector_alignment_shift;
    uint16_t resource_entries_in_table;
    uint8_t  executable_type;
    uint8_t  reserved [9];
} ne_header_t PACKED_STRUCT;
#pragma pack()

ne_header_t* get_ne_header(FILE* stream, uint32_t offset);
void_t       del_ne_header(ne_header_t* ne_header);

// LE header
//
// 0x0000 : "LE"    : 0x4C 0x45
// ...

#define LE_HEADER_SYNC 0x454C

#pragma pack(1)
typedef struct _le_header_t {
    uint16_t syncword;
} le_header_t PACKED_STRUCT;
#pragma pack()

le_header_t* get_le_header(FILE* stream, uint32_t offset);
void_t       del_le_header(le_header_t* le_header);

// LX header
//
// 0x0000 : "LX"    : 0x4C 0x58
// ...

#define LX_HEADER_SYNC 0x584C

#pragma pack(1)
typedef struct _lx_header_t {
    uint16_t syncword;
} lx_header_t PACKED_STRUCT;
#pragma pack()

lx_header_t* get_lx_header(FILE* stream, uint32_t offset);
void_t       del_lx_header(lx_header_t* lx_header);

// PE header
//
// 0x0000 : "PE"    : 0x50 0x45
// ...

#define PE_HEADER_SYNC 0x4550

#pragma pack(1)
typedef struct _pe_header_t {
    uint16_t syncword;
} pe_header_t PACKED_STRUCT;
#pragma pack()

pe_header_t* get_pe_header(FILE* stream, uint32_t offset);
void_t       del_pe_header(pe_header_t* pe_header);

// Windows executable file format
//
// 0x0000 : MZ header and reserved bytes
// 0x003C : Offset to segmented header (SEGHDR)
// 0x0040 : Relocation table
// 0x0200 : DOS data ("This program requires Microsoft Windows", etc)
// SEGHDR : Segmented header
// ...

#define SEGMENTED_HEADER_OFFSET 0x3C
#define RELOCATION_TABLE_OFFSET 0x40

typedef struct _exe_info_t {
    mz_header_t* mz_header;
    ne_header_t* ne_header;
    le_header_t* le_header;
    lx_header_t* lx_header;
    pe_header_t* pe_header;
    uint32_t     data_offset;
    uint32_t     extra_offset;
    bool_e       is_segmented;
    uint32_t     segmented_offset;
    uint16_t     segmented_syncword;
} exe_info_t;

exe_info_t* get_exe_info(FILE* stream, uint32_t offset);
void_t      del_exe_info(exe_info_t* exe_info);

// Entry of resource type
//
// 0x0000 : 2 bytes : Type ID. If high-order bit is set (0x8000) then it is integer
//                    Otherwise, it is offset to the type string (relative to beginning of resource table)
//                    Zero marks the end of information block
// 0x0002 : 2 bytes : Number of resources for this type
// 0x0004 : 4 bytes : Reserved

// Entry of resources info
//
// 0x0000 : 2 bytes : Offset to contents of the resource data (relative to beginning of file)
//                    Offset is in terms of the alignment shift count value
// 0x0002 : 2 bytes : Size of contents (in terms of the alignment shift count value)
// 0x0004 : 2 bytes : Flags:
//                    0x0010 = MOVEABLE  Resource is not fixed
//                    0x0020 = PURE      Resource can be shared
//                    0x0040 = PRELOAD   Resource is preloaded
// 0x0006 : 2 bytes : Resource ID. If high-order bit is set (0x8000) then it is integer
//                    Otherwise, it is offset to the resource string (relative to beginning of resource table)
// 0x0008 : 4 bytes : Reserved

// Resource table
//
// 0x0000 : 2 bytes      : Alignment shift count for resource data
// 0x0002 : 8 or 2 bytes : Resource type struct (2 bytes only if type ID = 0x0000)
// 0x000A : 12 bytes     : Resource info struct
// ...
// 0xXXXX : 1 byte       : ID string length (N bytes)
// 0xXXXY : N bytes      : ID string (it is not null terminated)
// ...

#define RESOURCE_TYPE_SIZE 0x08
#define RESOURCE_INFO_SIZE 0x0C

#pragma pack(1)
typedef struct _resource_type_t {
    uint16_t type_id;
    uint16_t num_of_resources;
    uint32_t reserved;
} resource_type_t PACKED_STRUCT;

typedef struct _resource_info_t {
    uint16_t content_offset;
    uint16_t content_size;
    uint16_t flags;
    uint16_t resource_id;
    uint32_t reserved;
} resource_info_t PACKED_STRUCT;
#pragma pack()

typedef struct _resource_entry_t {
    uint16_t type_id;
    uint16_t resource_id;
    uint16_t flags;
    uint32_t content_offset;
    uint32_t content_size;
} resource_entry_t;

typedef struct _resource_table_info_t {
    uint16_t          resource_data_alignment_shift;
    uint32_t          info_entries_num;
    resource_entry_t* info_entries;
} resource_table_info_t;

resource_table_info_t* get_resource_table_info(FILE* stream, uint32_t offset);
void_t                 del_resource_table_info(resource_table_info_t* resource_table_info);

// Resident-name table and nonresident-name table
//
// 0x0000 : 1 byte  : Name string length (N bytes)
//                    Zero marks the end of table
// 0x0001 : N bytes : Name string (it is not null terminated)
// 0xXXXX : 2 bytes : Ordinal number (index into entry table)
// ...

typedef struct _resident_info_t {
    uint32_t name_str_offset;
    uint16_t ordinal_number;
} resident_info_t;

typedef struct _resident_table_info_t {
    uint32_t         info_entries_num;
    resident_info_t* info_entries;
} resident_table_info_t;

resident_table_info_t* get_resident_table_info(FILE* stream, uint32_t offset);
void_t                 del_resident_table_info(resident_table_info_t* resident_table_info);

// Fixed segment entry
//
// 0x0000 : 1 byte  : Flags:
//                    0x01 = Set if entry is exported
//                    0x02 = Set if entry uses global (shared) data segments
// 0x0001 : 2 bytes : Offset within segment to entry point

// Moveable segment entry
//
// 0x0000 : 1 byte  : Flags:
//                    0x01 = Set if entry is exported
//                    0x02 = Set if entry uses global (shared) data segments
// 0x0001 : 2 bytes : INT 3FH
// 0x0003 : 1 byte  : Segment number
// 0x0004 : 2 bytes : Offset within segment to entry point

// Entry table
//
// 0x0000 : 1 byte  : Number of entries in current bundle
//                    Zero marks the end of table
// 0x0001 : 1 byte  : Segment indicator for current bundle:
//                    0x00 = Unused entry
//                    0x01
//                    ...  = Segment number for fixed segment entry
//                    0xFE
//                    0xFF = Moveable segment entry
// 0x0002 : N bytes : Segment entries
// ...

#define FIX_SEGMENT_ENTRY_SIZE 0x03
#define MOV_SEGMENT_ENTRY_SIZE 0x06

#pragma pack(1)
typedef struct _fixed_segment_entry_t {
    uint8_t  flags;
    uint16_t offset;
} fixed_segment_entry_t PACKED_STRUCT;

typedef struct _moveable_segment_entry_t {
    uint8_t  flags;
    uint16_t moveable_word;
    uint8_t  segment_num;
    uint16_t offset;
} moveable_segment_entry_t PACKED_STRUCT;
#pragma pack()

typedef struct _entries_bundle_t {
    uint8_t                   segment_indicator;
    uint8_t                   segment_entries_num;
    moveable_segment_entry_t* segment_entries;
} entries_bundle_t;

typedef struct _entry_table_info_t {
    uint32_t          entry_bundles_num;
    entries_bundle_t* entry_bundles;
} entry_table_info_t;

entry_table_info_t* get_entry_table_info(FILE* stream, uint32_t offset);
void_t              del_entry_table_info(entry_table_info_t* entry_table_info);

#endif // __EXE_HEAD_H__
