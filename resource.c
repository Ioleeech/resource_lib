#include <stdio.h>
#include <stdlib.h>

#include "inttypes.h"
#include "platform.h"

#include "resource.h"

static const char_t* resource_type_str [RT_MAX_NUM] = {
    "UNKNOWN (0)",
    "RT_CURSOR",
    "RT_BITMAP",
    "RT_ICON",
    "RT_MENU",
    "RT_DIALOG",
    "RT_STRING",
    "RT_FONTDIR",
    "RT_FONT",
    "RT_ACCELERATOR",
    "RT_RCDATA",
    "RT_MESSAGETABLE",
    "RT_GROUP_CURSOR",
    "UNKNOWN (13)",
    "RT_GROUP_ICON",
    "UNKNOWN (15)",
    "RT_VERSION",
    "RT_DLGINCLUDE",
    "UNKNOWN (18)",
    "RT_PLUGPLAY",
    "RT_VXD",
    "RT_ANICURSOR",
    "RT_ANIICON",
    "RT_HTML",
    "RT_MANIFEST"
};

const char_t* convert_resource_type_id_to_text(uint16_t type_id)
{
    // Clear integer bit
    type_id &= ~0x8000;

    // Convert ID to text
    return (type_id < RT_MAX_NUM) ? resource_type_str[type_id] : NULL;
}
