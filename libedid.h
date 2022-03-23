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

#ifndef LIB_EDID_H
#define LIB_EDID_H

#include <sys/types.h>

enum video_scanning {
        alwyas_underscanned = 1,
        alwyas_overscanned,
        both_over_and_under_scanned,
};

enum edid_clr_depth {
        CLR_DEPTH_6BPC = 1,
        CLR_DEPTH_8BPC,
        CLR_DEPTH_10BPC,
        CLR_DEPTH_12BPC,
        CLR_DEPTH_14BPC,
        CLR_DEPTH_16BPC,
};

enum cea_data_block_extended_tags {
        /* Video Capability Data Block */
        CEA_DATA_BLOCK_EXT_VCDB = 0,
        /* Vendor-Specific Video Data Block */
        CEA_DATA_BLOCK_EXT_VSVDB,
        /* VESA Display Device Data Block */
        CEA_DATA_BLOCK_EXT_VDDDB,
        /*VESA Video Timing Block Extension */
        CEA_DATA_BLOCK_EXT_VVTBE,
        /* Reserved for HDMI Video Data Block */
        CEA_DATA_BLOCK_EXT_HDMI_VDB,
        /* Colorimetry Data Block */
        CEA_DATA_BLOCK_EXT_COLORIMETRY,
        /* HDR Static Metadata Data Block */
        CEA_DATA_BLOCK_EXT_HDR_STATIC_MD,
        /* HDR Dynamic Metadata Data Block */
        CEA_DATA_BLOCK_EXT_HDR_DYNAMIC_MD,
        /* Video Format Preference Data Block */
        CEA_DATA_BLOCK_EXT_VF_PREF = 13,
        /* YC B C R 4:2:0 Video Data Block */
        CEA_DATA_BLOCK_EXT_YCBCR420_VDB,
        /* YC B C R 4:2:0 Capability Map Data Block */
        CEA_DATA_BLOCK_EXT_YCBCR420_CMDB,
        /* Reserved for CTA Miscellaneous Audio Fields */
        CEA_DATA_BLOCK_EXT_RES_MISC_AUD,
        /* Vendor-Specific Audio Data Block */
        CEA_DATA_BLOCK_EXT_VSADB,
        /* Reserved for HDMI Audio Data Block */
        CEA_DATA_BLOCK_EXT_RES_HDMI_ADB,
        /* Room Configuration Data Block */
        CEA_DATA_BLOCK_EXT_RCDB,
        /* Speaker Location Data Block */
        CEA_DATA_BLOCK_EXT_SPKR_LOC_DB,
        /* InfoFrame Data Block (includes one or more Short InfoFrame Descriptors) */
        CEA_DATA_BLOCK_EXT_IFDB = 32,
};

enum cea_data_block_tags {
        CEA_DATA_BLOCK_RESERVED = 0,
        CEA_DATA_BLOCK_AUDIO,
        CEA_DATA_BLOCK_VIDEO,
        CEA_DATA_BLOCK_VENDOR,
        CEA_DATA_BLOCK_SPEAKER_ALLOC,
        CEA_DATA_BLOCK_VESA_DTC,
        CEA_DATA_BLOCK_RESERVED_2,
        CEA_DATA_BLOCK_EXTENDED,
};

struct cea_vsdb {
        u_int32_t oui;
        u_int8_t datalen;

        /* free this */
        u_int8_t *data;
};

struct cea_colorimetry {
        /* Colorimetry based on IEC 61966-2-5 */
        u_int8_t opRGB;
        u_int8_t opYCC_601;

        /* Colorimetry based on IEC 61966-2-1/Amendment 1 */
        u_int8_t sYCC_601;

        /* Standard Definition Colorimetry based on IEC 61966-2-4 */
        u_int8_t xvYCC_601;

        /* High Definition Colorimetry based on IEC 61966-2-4 */
        u_int8_t xvYCC_709;

        /* Colorimetry based on ITU-R BT.2020 */
        u_int8_t BT2020_RGB;
        u_int8_t BT2020_YCC;
        u_int8_t BT2020_CYCC;

        /* Colorimetry based on DCI-P3 */
        u_int8_t DCIP3;

};

struct cea_dynamic_hdr_md {
        u_int8_t size;
        /* free this */
        u_int8_t *data;
};

struct cea_static_hdr_md {

        /* This is the content’s absolute peak luminance */
        float content_max_lum;

        /* This is the content’s max frame-average luminance */
        float content_fav_lum;

        /* This is the minimum value of the content luminance */
        float content_min_lum;

        /* EOTF: Traditional gamma SDR curve */
        u_int8_t gamma_sdr;

        /* EOTF: Traditional gamma HDR curve */
        u_int8_t gamma_hdr;

        /* EOTF: SMPTE ST2084 gamma curve */
        u_int8_t gamma_st2084;

        /* EOTF: BT2100 Hybrid log gamma curve */
        u_int8_t gamma_hlg;

        /* Sink supports static metadata type 1 */
        u_int8_t static_md_type_1;
};

struct cea_ifdb {
        u_int8_t data_len;
        u_int8_t num_vsif;
        /* free this */
        u_int8_t *data;
};

struct cea_video_caps {
        u_int8_t quant_range_selectable_ycc;
        u_int8_t quant_range_selectable_rgb;
        enum video_scanning pt;
        enum video_scanning it;
        enum video_scanning ce;
};

struct vsdb_phy {
        u_int8_t a;
        u_int8_t b;
        u_int8_t c;
        u_int8_t d;
};

struct hdmi_vsdb {

        /* deep color */
        u_int8_t dc_48_bpc;
        u_int8_t dc_36_bpc;
        u_int8_t dc_30_bpc;
        u_int8_t dc_ycbcr444;

        /* Supports ACP, ISRC1, or ISRC2 packets */
        u_int8_t ai;

        /* DVI-dual link */
        u_int8_t dvi_dual;

        /* Max pixel clock support in Mhz */
        u_int32_t max_tmds_clock_mhz;

        /* Source physcial addrress A.B.C.D */
        struct vsdb_phy phy;
};

struct hf_vsdb {
        /* HDMI 2.0 features */
        u_int32_t version;
        u_int8_t scdc;
        u_int8_t rr;
        u_int32_t max_tmds_rate_mhz;
        u_int8_t scrambling_340mhz;

        u_int8_t iv;
        u_int8_t dv;
        u_int8_t osd_3d;

        /* Deep color support in YCBCR 4:2:0 mode */
        u_int8_t dc_48_420;
        u_int8_t dc_36_420;
        u_int8_t dc_30_420;
};

enum stereo_mode_type {
        STEREO_MODE_NONE = 0,
        STEREO_MODE_SEQ_RIGHT = 2,
        STEREO_MODE_2W_INTERL_RIGHT,
        STEREO_MODE_SEQ_LEFT,
        STEREO_MODE_2W_INTERL_LEFT,
        STEREO_MODE_4W_INTERL,
        STEREO_MODE_SBS_RIGHT,
};

struct detailed_mode {
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
        enum stereo_mode_type stereo;
};

struct dtd_blk {
        u_int8_t n_dtd_modes;
        /* Free this */
        struct detailed_mode *d_modes;
};

struct edid_tags {
        u_int8_t revision;
        u_int8_t n_cea_ext_blks;
        u_int8_t n_dtd_blks;
        u_int8_t audio;
        u_int8_t it_underscan;
        u_int8_t ycbcr444;
        u_int8_t ycbcr422;

        /* Bitmap representing supported CEA video modes
         * with VIC 1-255, a bit set means it's corresponding
         * VIC is supported. Bit 0 means VIC 1, 254 is VIC 255.
         */
        u_int64_t vics[4];
        u_int8_t native_vic;

        /* This bitmap represents YCBCR 4:2:0 only VICs, ie, video modes
         * which can be supported only in YCBCR 4:2:0 sampling. Note
         */
        u_int64_t vics_420_only[4];

        /* This bitmap represents YCBCR 4:2:0 also VICs, ie, video modes
         * which can support YCBCR 4:2:0 sampling also, along with oters
         * like RGB/YCBCR44/422 etc.
         */
        u_int64_t vics_420_also[4];

        /* VICs as per the Video format preference block (listing max 10)
         * These should take the highest priority than any other block. If
         * a sink wants to prefer a YCBCR420 block above others, this is
         * the place to do it.
         */
        u_int8_t vics_preferred[10];

        /* Vendor specific data dynamic ptr, free .data ptr in this */
        struct cea_vsdb vsdb;

        /* Vendor specific video data block, free .data ptr in this */
        struct cea_vsdb vsvdb;

        /* Colorimetry information */
        struct cea_colorimetry colorimetry;

        /* Video capability */
        struct cea_video_caps vcap;

        /* Infoframe data block */
        struct cea_ifdb ifdb;

        /* Static HDR metadata */
        struct cea_static_hdr_md hdr_smd;

        /* Dynamic HDR metadata, must be freed */
        struct cea_dynamic_hdr_md hdr_dmd;

        /* HDMI Forum VSDB */
        struct hf_vsdb hfvsdb;

        /* HDMI VSDB */
        struct hdmi_vsdb hdmi_vsdb;

        /* Detailed timing modes */
        struct dtd_blk dtd;
};

/* Base edid block */
struct edid_supp_clr_formats {
        u_int8_t clr_format_rgb444;
        u_int8_t clr_format_ycbcr444;
        u_int8_t clr_format_ycbcr422;
        u_int8_t clr_format_ycbcr420;
};

struct edid_base_blk {
        u_int8_t srgb_default;
        u_int8_t standby;
        u_int8_t suspend;
        u_int8_t active_off;
        u_int32_t pid;
        u_int32_t sno;
        char vendor[4];
        enum edid_clr_depth clr_depth;
        struct edid_supp_clr_formats clr_formats;
        float gamma;
        struct detailed_mode dmodes[4];
};

struct edid_info {
        u_int8_t *raw_edid;
        struct edid_tags cea_blks;
        struct edid_base_blk base_blk;
};

void libedid_destroy_edid_info(struct edid_info *info);
struct edid_info *libedid_process_edid_info(u_int8_t *raw_edid);

#endif