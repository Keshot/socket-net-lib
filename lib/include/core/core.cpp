#include "core.h"

i32 check_byte_order()
{
    short t = 0x0102;
    u8 *tx = ccast(u8*, &t);
    return *tx == 0x01 ? BIG_ENDIAN : LITTLE_ENDIAN;
}