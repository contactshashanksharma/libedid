/*
 * Copyright (c) 2021 Shashank Sharma (contactshashanksharma@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <sys/types.h>
#include <stdio.h>
#include <sys/types.h>

#include "libedid.h"

static u_int8_t static_edid_lg[] = {

        /* Base block */
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
	0x1E,0x6D,0x06,0x77,0x0A,0xAC,0x0A,0x00,
	0x07,0x1E,0x01,0x03,0x80,0x3C,0x22,0x78,
	0xEA,0x3E,0x31,0xAE,0x50,0x47,0xAC,0x27,
	0x0C,0x50,0x54,0x21,0x08,0x00,0x71,0x40,
	0x81,0x80,0x81,0xC0,0xA9,0xC0,0xD1,0xC0,
	0x81,0x00,0x01,0x01,0x01,0x01,0x08,0xE8,
	0x00,0x30,0xF2,0x70,0x5A,0x80,0xB0,0x58,
	0x8A,0x00,0x58,0x54,0x21,0x00,0x00,0x1E,
	0x04,0x74,0x00,0x30,0xF2,0x70,0x5A,0x80,
	0xB0,0x58,0x8A,0x00,0x58,0x54,0x21,0x00,
	0x00,0x1A,0x00,0x00,0x00,0xFD,0x00,0x28,
	0x3D,0x1E,0x87,0x3C,0x00,0x0A,0x20,0x20,
	0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,
	0x00,0x4C,0x47,0x20,0x48,0x44,0x52,0x20,
	0x34,0x4B,0x0A,0x20,0x20,0x20,0x01,0x29,
	
	/* CEA Extension block */
	0x02,0x03,0x44,0x71,0x4D,0x90,0x22,0x20,
	0x1F,0x12,0x03,0x04,0x01,0x61,0x60,0x5D,
	0x5E,0x5F,0x23,0x09,0x07,0x07,0x6D,0x03,
	0x0C,0x00,0x10,0x00,0xB8,0x3C,0x20,0x00,
	0x60,0x01,0x02,0x03,0x67,0xD8,0x5D,0xC4,
	0x01,0x78,0x80,0x03,0xE3,0x0F,0x00,0x03,
	0x68,0x1A,0x00,0x00,0x01,0x01,0x28,0x3D,
	0x00,0xE3,0x05,0xC0,0x00,0xE6,0x06,0x05,
	0x01,0x52,0x48,0x5D,0x02,0x3A,0x80,0x18,
	0x71,0x38,0x2D,0x40,0x58,0x2C,0x45,0x00,
	0x58,0x54,0x21,0x00,0x00,0x1E,0x56,0x5E,
	0x00,0xA0,0xA0,0xA0,0x29,0x50,0x30,0x20,
	0x35,0x00,0x58,0x54,0x21,0x00,0x00,0x1A,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF1
};


static u_int8_t static_edid_dell[] = {

        /* Base block */
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
	0x10,0xAC,0xBC,0xA0,0x55,0x52,0x31,0x32,
	0x1C,0x1D,0x01,0x03,0x80,0x34,0x20,0x78,
	0xEA,0x04,0x95,0xA9,0x55,0x4D,0x9D,0x26,
	0x10,0x50,0x54,0xA5,0x4B,0x00,0x71,0x4F,
	0x81,0x80,0xA9,0x40,0xD1,0xC0,0xD1,0x00,
	0x01,0x01,0x01,0x01,0x01,0x01,0x28,0x3C,
	0x80,0xA0,0x70,0xB0,0x23,0x40,0x30,0x20,
	0x36,0x00,0x06,0x44,0x21,0x00,0x00,0x1E,
	0x00,0x00,0x00,0xFF,0x00,0x56,0x57,0x36,
	0x31,0x31,0x39,0x37,0x38,0x32,0x31,0x52,
	0x55,0x0A,0x00,0x00,0x00,0xFC,0x00,0x44,
	0x45,0x4C,0x4C,0x20,0x55,0x32,0x34,0x31,
	0x35,0x0A,0x20,0x20,0x00,0x00,0x00,0xFD,
	0x00,0x31,0x3D,0x1E,0x53,0x11,0x00,0x0A,
	0x20,0x20,0x20,0x20,0x20,0x20,0x01,0x9E,

        /* CEA Extension block */
	0x02,0x03,0x22,0xF1,0x4F,0x90,0x05,0x04,
	0x03,0x02,0x07,0x16,0x01,0x14,0x1F,0x12,
	0x13,0x20,0x21,0x22,0x23,0x09,0x07,0x07,
	0x65,0x03,0x0C,0x00,0x20,0x00,0x83,0x01,
	0x00,0x00,0x02,0x3A,0x80,0x18,0x71,0x38,
	0x2D,0x40,0x58,0x2C,0x45,0x00,0x06,0x44,
	0x21,0x00,0x00,0x1E,0x01,0x1D,0x80,0x18,
	0x71,0x1C,0x16,0x20,0x58,0x2C,0x25,0x00,
	0x06,0x44,0x21,0x00,0x00,0x9E,0x01,0x1D,
	0x00,0x72,0x51,0xD0,0x1E,0x20,0x6E,0x28,
	0x55,0x00,0x06,0x44,0x21,0x00,0x00,0x1E,
	0x8C,0x0A,0xD0,0x8A,0x20,0xE0,0x2D,0x10,
	0x10,0x3E,0x96,0x00,0x06,0x44,0x21,0x00,
	0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x72
};

int main()
{
        struct edid_info *info;

        /* Test LG EDID */
        info = libedid_process_edid_info(static_edid_lg);
        if (!info)
                printf("Failed to get edid infomrmation\n");
        else
                libedid_destroy_edid_info(info);

        /* Now DELL EDID */
        info = libedid_process_edid_info(static_edid_dell);
        if (!info)
                printf("Failed to get edid infomrmation\n");
        else
                libedid_destroy_edid_info(info);

        return 0;
}