
#include "Global.h"

u16 be16 (const u8 *p) {
	return (p[0] << 8) | p[1];
}

u32 be32 (const u8 *p) {
	return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

u64 be64 (const u8 *p) {
	return ((u64) be32 (p) << 32) | be32 (p + 4);
}

size_t g_strnlen (const char *s, size_t size) {
        size_t i;

        for (i = 0; i < size; ++i)
                if (s[i] == 0)
                        return i;

        return size;
}

u32 get_dol_size (const u8 *header) {
        u8 i;
        u32 offset, size;
        u32 max = 0;

        // iterate through the 7 code segments
        for (i = 0; i < 7; ++i) {
                offset = be32 (&header[i * 4]);
                size = be32 (&header[0x90 + i * 4]);
                if (offset + size > max)
                        max = offset + size;
        }

        // iterate through the 11 data segments
        for (i = 0; i < 11; ++i) {
                offset = be32 (&header[0x1c + i * 4]);
                size = be32 (&header[0xac + i * 4]);
                if (offset + size > max)
                        max = offset + size;
        }

        return max;
}