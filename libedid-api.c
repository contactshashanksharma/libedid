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

#include <sys/types.h>
#include <stdio.h>
#include <stdbool.h>

#include "libedid-api.h"
#include "libedid.h"

char *libedid_get_display_vendor(void *edid_info)
{
    struct edid_info *info = edid_info;

    return (char*)&info->base_blk.vendor;
}

unsigned int libedid_get_display_productid(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->base_blk.pid;
}

unsigned int libedid_get_display_sno(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->base_blk.sno;
}

struct libedid_detailed_mode *libedid_get_preferred_mode(void *edid_info)
{
    struct edid_info *info = edid_info;

    /* First detailed mode of EDID is supposed to be preferred */
    return (struct libedid_detailed_mode *)&info->base_blk.dmodes[0];
}

bool libedid_display_supports_ycbcr(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->base_blk.clr_formats.clr_format_ycbcr420 ||
            info->base_blk.clr_formats.clr_format_ycbcr422 ||
            info->base_blk.clr_formats.clr_format_ycbcr444;

}

bool libedid_display_supports_ycbcr444(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->base_blk.clr_formats.clr_format_ycbcr444;
}

bool libedid_display_supports_ycbcr422(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->base_blk.clr_formats.clr_format_ycbcr422;
}

bool libedid_display_supports_ycbcr420(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->base_blk.clr_formats.clr_format_ycbcr420;
}

bool libedid_display_supports_dcip3(void *edid_info)
{
    struct edid_info *info = edid_info;
    
    return info->cea_blks.colorimetry.DCIP3;
}

bool libedid_display_supports_bt2020(void *edid_info)
{
    struct edid_info *info = edid_info;
    
    return info->cea_blks.colorimetry.BT2020_RGB ||
            info->cea_blks.colorimetry.BT2020_YCC ||
            info->cea_blks.colorimetry.BT2020_CYCC;
}

bool libedid_display_supports_dc(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.hdmi_vsdb.dc_30_bpc ||
            info->cea_blks.hdmi_vsdb.dc_36_bpc ||
            info->cea_blks.hdmi_vsdb.dc_48_bpc;
}

bool libedid_display_supports_dc_10bpc(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.hdmi_vsdb.dc_30_bpc;
}

bool libedid_display_supports_dc_12bpc(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.hdmi_vsdb.dc_36_bpc;
}

bool libedid_display_supports_dc_16bpc(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.hdmi_vsdb.dc_48_bpc;
}

bool libedid_display_supports_dc420(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.hfvsdb.dc_30_420 ||
            info->cea_blks.hfvsdb.dc_36_420 ||
            info->cea_blks.hfvsdb.dc_48_420;
}

unsigned char libedid_display_deepest_420_color_depth(void *edid_info)
{
    struct edid_info *info = edid_info;

    if (info->cea_blks.hfvsdb.dc_48_420)
        return 16;
    if (info->cea_blks.hfvsdb.dc_36_420)
        return 12;
    if (info->cea_blks.hfvsdb.dc_30_420)
        return 10;
    /* 0 indicates no deep color support in hf_vsdb */
    return 0;
}

bool libedid_display_supports_dc420_10bpc(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.hfvsdb.dc_30_420;
}

bool libedid_display_supports_dc420_12bpc(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.hfvsdb.dc_36_420;
}

bool libedid_display_supports_dc420_16bpc(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.hfvsdb.dc_48_420;
}

unsigned int libedid_display_max_tmds_clk_mhz(void *edid_info)
{
    struct edid_info *info = edid_info;

    /* HF-VSDB value gets the priority */
    if (info->cea_blks.hfvsdb.max_tmds_rate_mhz)
        return info->cea_blks.hfvsdb.max_tmds_rate_mhz;

    return info->cea_blks.hdmi_vsdb.max_tmds_clock_mhz;
}

bool libedid_display_supports_audio(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.audio;
}

double libedid_display_hdr_max_lum(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.hdr_smd.content_max_lum;
}

double libedid_display_hdr_min_lum(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.hdr_smd.content_min_lum;
}

bool libedid_display_supports_hdr_output(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.hdr_smd.gamma_hdr ||
        info->cea_blks.hdr_smd.gamma_hlg ||
        info->cea_blks.hdr_smd.gamma_st2084;
}

bool libedid_display_supports_hdr_gamma(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.hdr_smd.gamma_hdr;
}

bool libedid_display_supports_hdr_st2084(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.hdr_smd.gamma_st2084;
}

bool libedid_display_supports_hdr_hlg(void *edid_info)
{
    struct edid_info *info = edid_info;

    return info->cea_blks.hdr_smd.gamma_hlg;
}

void *libedid_init(unsigned char *raw_edid)
{
    struct edid_info *info;

    if (!raw_edid)
        return NULL;

    info = libedid_process_edid_info(raw_edid);
    return info; 
}

void libedid_destroy(void *info)
{
    if (info)
        libedid_destroy_edid_info((struct edid_info *)info);
}