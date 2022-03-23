/*
 * Copyright (c) 2022 Shashank Sharma (contactshashanksharma@gmail.com)
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

#ifndef LIB_EDID_API_H
#define LIB_EDID_API_H

#include <stdlib.h>

enum edid_stereo_type {
        EDID_STEREO_MODE_NONE = 0,
        EDID_STEREO_MODE_SEQ_RIGHT = 2,
        EDID_STEREO_MODE_2W_INTERL_RIGHT,
        EDID_STEREO_MODE_SEQ_LEFT,
        EDID_STEREO_MODE_2W_INTERL_LEFT,
        EDID_STEREO_MODE_4W_INTERL,
        EDID_STEREO_MODE_SBS_RIGHT,
};

struct libedid_detailed_mode {
        u_int32_t pixel_clock_khz;
        u_int32_t hactive;
        u_int16_t hblank;
        u_int16_t hfrontp;
        u_int16_t hsync;
        u_int16_t hsize_mm;
        u_int16_t hborder_2;

        u_int32_t vactive;
        u_int16_t vblank;
        u_int16_t vfrontp;
        u_int16_t vsync;
        u_int16_t vsize_mm;
        u_int16_t vborder_2;

        u_int8_t interlaced;
        enum edid_stereo_type stereo;
};

char *libedid_get_display_vendor(void *edid_info);

unsigned int libedid_get_display_productid(void *edid_info);

unsigned int libedid_get_display_sno(void *edid_info);

struct libedid_detailed_mode *libedid_get_preferred_mode(void *edid_info);

bool libedid_display_supports_ycbcr(void *edid_info);

bool libedid_display_supports_ycbcr444(void *edid_info);

bool libedid_display_supports_ycbcr422(void *edid_info);

bool libedid_display_supports_ycbcr420(void *edid_info);

bool libedid_display_supports_dcip3(void *edid_info);

bool libedid_display_supports_bt2020(void *edid_info);

bool libedid_display_supports_dc(void *edid_info);

bool libedid_display_supports_dc_10bpc(void *edid_info);

bool libedid_display_supports_dc_12bpc(void *edid_info);

bool libedid_display_supports_dc_16bpc(void *edid_info);

bool libedid_display_supports_dc420(void *edid_info);

unsigned char libedid_display_deepest_420_color_depth(void *edid_info);

bool libedid_display_supports_dc420_10bpc(void *edid_info);

bool libedid_display_supports_dc420_12bpc(void *edid_info);

bool libedid_display_supports_dc420_16bpc(void *edid_info);

unsigned int libedid_display_max_tmds_clk_mhz(void *edid_info);

bool libedid_display_supports_audio(void *edid_info);

double libedid_display_hdr_max_lum(void *edid_info);

double libedid_display_hdr_min_lum(void *edid_info);

bool libedid_display_supports_hdr_gamma(void *edid_info);

bool libedid_display_supports_hdr_st2084(void *edid_info);

bool libedid_display_supports_hdr_hlg(void *edid_info);

bool libedid_display_supports_hdr_output(void *edid_info);

void *libedid_init(unsigned char *raw_edid);

void libedid_destroy(void *info);

#endif