/*
 *  Copyright (C) 2008 dhewg, #wiidev efnet
 *
 *  this file is part of wiifuse
 *  http://wiibrew.org/index.php?title=Wiifuse
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#define ROUNDUP64B(x) (((u64)(x) + 64 - 1) & ~(64 - 1))

u16 be16 (const u8 *p);
u32 be32 (const u8 *p);
u64 be64 (const u8 *p);

extern u8 verbose_level;

size_t g_strnlen (const char *s, size_t size);

u32 get_dol_size (const u8 *header);