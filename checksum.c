#include <stdio.h>
#include <stdlib.h>

#include "inttypes.h"
#include "platform.h"

#include "checksum.h"

uint16_t validate_mz_checksum(FILE* stream, uint32_t size)
{
    uint16_t checksum = 0x0000;

    if ((size < sizeof(uint16_t)) || (fseek(stream, 0, SEEK_SET) != 0))
        return checksum;

    for ( ; ; )
    {
        uint16_t word = 0x0000;

        if (fread(&word, 1, sizeof(uint16_t), stream) != sizeof(uint16_t))
            break;

        checksum += word;
        size     -= sizeof(uint16_t);

        if (! size)
            break;

        if (size < sizeof(uint16_t))
        {
            uint8_t byte = 0x00;

            if (fread(&byte, 1, 1, stream) == 1)
                checksum += byte;

            break;
        }
    }

    return checksum; // Must returns 0xFFFF if checksum is OK
}
