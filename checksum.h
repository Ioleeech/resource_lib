#ifndef __CHECKSUM_H__
#define __CHECKSUM_H__

#include <stdio.h>

#include "inttypes.h"
#include "platform.h"

uint16_t validate_mz_checksum(FILE* stream, uint32_t size);

#endif // __CHECKSUM_H__
