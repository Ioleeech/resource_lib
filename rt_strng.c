#include <stdio.h>
#include <stdlib.h>

#include "inttypes.h"
#include "platform.h"

#include "rt_strng.h"

rt_string_t* get_calculated_string(FILE* stream, uint32_t offset)
{
    if (offset == CURRENT_OFFSET)
    {
        sint32_t current = ftell(stream);

        if (current < 0)
            return NULL;
        else
            offset = current;
    }

    if (fseek(stream, offset, SEEK_SET) != 0)
        return NULL;

    // Get text length
    uint8_t length = 0;

    if (fread(&length, 1, 1, stream) != 1)
        return NULL;

    if (! length)
        return NULL;

    // Get text
    char_t* ascii = (char_t*) malloc(length + 1);

    if (! ascii)
        return NULL;

    if (fread(ascii, 1, length, stream) != length)
    {
        free(ascii);
        return NULL;
    }

    // String must be null terminated
    ascii[length] = '\0';

    // Prepare rt_string struct
    rt_string_t* rt_string = (rt_string_t*) malloc(sizeof(rt_string_t));

    if (! rt_string)
    {
        free(ascii);
        return NULL;
    }

    rt_string->length = length;
    rt_string->ascii  = ascii;

    return rt_string;
}

rt_string_t* get_terminated_string(FILE* stream, uint32_t offset)
{
    if (offset == CURRENT_OFFSET)
    {
        sint32_t current = ftell(stream);

        if (current < 0)
            return NULL;
        else
            offset = current;
    }

    // Get text length
    uint32_t length;

    if (fseek(stream, offset, SEEK_SET) != 0)
        return NULL;

    for (length = 0 ; ; length ++)
    {
        char_t symbol = '\0';

        if (fread(&symbol, 1, sizeof(char_t), stream) != sizeof(char_t))
            return NULL;

        if (symbol == '\0')
            break;
    }

    if (! length)
        return NULL;

    // Get text (string is already null terminated)
    char_t* ascii = (char_t*) malloc(length + 1);

    if (! ascii)
        return NULL;

    if (fseek(stream, offset, SEEK_SET) != 0)
    {
        free(ascii);
        return NULL;
    }

    if (fread(ascii, 1, (length + 1), stream) != (length + 1))
    {
        free(ascii);
        return NULL;
    }

    // Prepare rt_string struct
    rt_string_t* rt_string = (rt_string_t*) malloc(sizeof(rt_string_t));

    if (! rt_string)
    {
        free(ascii);
        return NULL;
    }

    rt_string->length = length;
    rt_string->ascii  = ascii;

    return rt_string;
}

void_t del_rt_string(rt_string_t* rt_string)
{
    free(rt_string->ascii);
    free(rt_string);
}
