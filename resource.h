#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <stdio.h>

#include "inttypes.h"
#include "platform.h"

typedef enum _resource_types_e {
    RT_UNKNOWN_0    =  0,
    RT_CURSOR       =  1, // Hardware-dependent cursor resource
    RT_BITMAP       =  2, // Bitmap resource
    RT_ICON         =  3, // Hardware-dependent icon resource
    RT_MENU         =  4, // Menu resource
    RT_DIALOG       =  5, // Dialog box
    RT_STRING       =  6, // String-table entry
    RT_FONTDIR      =  7, // Font directory resource
    RT_FONT         =  8, // Font resource
    RT_ACCELERATOR  =  9, // Accelerator table
    RT_RCDATA       = 10, // Application-defined resource (raw data)
    RT_MESSAGETABLE = 11, // Message-table entry
    RT_GROUP_CURSOR = 12, // Hardware-independent cursor resource
    RT_UNKNOWN_13   = 13,
    RT_GROUP_ICON   = 14, // Hardware-independent icon resource
    RT_UNKNOWN_15   = 15,
    RT_VERSION      = 16, // Version resource
    RT_DLGINCLUDE   = 17, // Allows a resource editing tool to associate a string with an .rc file
    RT_UNKNOWN_18   = 18,
    RT_PLUGPLAY     = 19, // Plug and Play resource
    RT_VXD          = 20, // VXD
    RT_ANICURSOR    = 21, // Animated cursor
    RT_ANIICON      = 22, // Animated icon
    RT_HTML         = 23, // HTML
    RT_MANIFEST     = 24, // Microsoft Windows XP: Side-by-Side Assembly XML Manifest
    RT_MAX_NUM
} resource_types_e;

const char_t* convert_resource_type_id_to_text(uint16_t type_id);

#endif // __RESOURCE_H__
