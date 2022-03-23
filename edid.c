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

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>

#include "libedid.h"
#include "edid_timing.h"

/* HDMI */
#define HDMI_IEEE_OUI 0x000C03
#define HDMI_FORUM_IEEE_OUI 0xC45DD8

/* HF VSDB Byte 6 */
#define HDVSDB_SCDC_BIT 7
#define HDVSDB_RR_BIT   6
#define HDVSDB_SCRAMBLING_AT_340_BIT 3
#define HDVSDB_IV_BIT 2
#define HDVSDB_DV_BIT 1
#define HDVSDB_3D_OSD_BIT 0

/* HF VSDB Byte 7 */
#define HDVSDB_DC_420_16BPC 2
#define HDVSDB_DC_420_12BPC 1
#define HDVSDB_DC_420_10BPC 0

/* HDMI VSDB Byte 6 */
#define HDVSDB_AI_BIT 7
#define HDVSDB_DC_16BPC 6
#define HDVSDB_DC_12BPC 5
#define HDVSDB_DC_10BPC 4
#define HDVSDB_DC_YCBCR444 3
#define HDVSDB_DVI_DUAL_BIT 0

/* CEA block */
#define CEA_EXT_IT_UNDESCAN_BIT 7
#define CEA_EXT_AUDIO_BIT       6
#define CEA_EXT_YCBCR444_BIT    5
#define CEA_EXT_YCBCR422_BIT    5

/* CEA extenstion block */
#define CEA_EXTN_BLK_SIZE 128
#define CEA_EXT_BLK_TAG_VALUE 0x2

/* CEA colorimetry block byte 1 */
#define CEA_EXT_CLR_BT2020RGB_BIT   7
#define CEA_EXT_CLR_BT2020YCC_BIT   6
#define CEA_EXT_CLR_BT2020CYCC_BIT  5
#define CEA_EXT_CLR_opRGB_BIT       4
#define CEA_EXT_CLR_opYCC601_BIT    3
#define CEA_EXT_CLR_sYCC601_BIT     2
#define CEA_EXT_CLR_xvYCC709_BIT    1
#define CEA_EXT_CLR_xvYCC601_BIT    0

/* CEA colorimetry block byte 2 */
#define CEA_EXT_CLR_DCIP3_BIT       7

/* CEA video capacity block handling */
#define CEA_VCAP_QUANT_RANGE_SELECT_YCC_BIT 7
#define CEA_VCAP_QUANT_RANGE_SELECT_RGB_BIT 6
#define CEA_VCAP_PT_SCANNING_MASK (0x3 << 4)
#define CEA_VCAP_PT_SCANNING_VAL(x) ((x & CEA_VCAP_PT_SCANNING_MASK) >> 4)
#define CEA_VCAP_IT_SCANNING_MASK (0x3 << 2)
#define CEA_VCAP_IT_SCANNING_VAL(x) ((x & CEA_VCAP_IT_SCANNING_MASK) >> 2)
#define CEA_VCAP_CE_SCANNING_MASK (0x3 << 0)
#define CEA_VCAP_CE_SCANNING_VAL(x) ((x & CEA_VCAP_CE_SCANNING_MASK) >> 0)

/* HDR Metadata byte 1 */
#define CEA_HDR_EOTF_SDR_BIT    0
#define CEA_HDR_EOTF_HDR_BIT    1
#define CEA_HDR_EOTF_ST2084_BIT 2
#define CEA_HDR_EOTF_HLG_BIT    3

/* HDR Metadata byte 2 */
#define CEA_HDR_SMD_TYPE1_BIT   0

/* Supported color formats */
#define EDID_CLR_FORMAT_YCBCR_444_BIT 0
#define EDID_CLR_FORMAT_YCBCR_422_BIT 1
#define EDID_CLR_FORMAT_YCBCR_420_BIT 2

/* Monitor features */
#define EDID_INPUT_IS_DIGITAL (1 << 7)
#define EDID_STANDBY_SUPPORTED(s) ((s & (1 << 7)) >> 7)
#define EDID_SUSPEND_SUPPORTED(s) ((s & (1 << 6)) >> 6)
#define EDID_ACTIVE_OFF_SUPPORTED(s) ((s & (1 << 5)) >> 5)
#define EDID_INPUT_CLR_DEPTH(i) ((i & (0x7 << 4)) >> 4)
#define EDID_INPUT_CLR_FORMAT(f) ((f & (0x3 << 3)) >> 3)
#define EDID_SRGB_CLRSP_DEFAULT(c) ((c & (1 << 2)) >> 2)

/* Deatailed timing descriptor */
#define DTD_FP_SHIFT 6
#define DTD_FP_MASK (0x3 << DTD_FP_SHIFT)
#define DTD_FP(a) ((a & DTD_FP_MASK) >> DTD_FP_SHIFT)

#define DTD_ACT_SHIFT 4
#define DTD_ACT_MASK 0xF0
#define DTD_ACT(a) ((a & DTD_ACT_MASK) >> DTD_ACT_SHIFT)

#define DTD_BL_MASK 0x0F
#define DTD_BL(a) (a & DTD_BL_MASK)

#define DTD_HSYNC_SHIFT 4
#define DTD_HSYNC_MASK (0x3 << DTD_HSYNC_SHIFT)
#define DTD_HSYNC(h) ((h & DTD_HSYNC_MASK) >> DTD_HSYNC_SHIFT)

#define DTD_VFP_H(v) ((v & (0x3 << 2)) >> 2)
#define DTD_VFP_L(v) ((v & (0xF << 4) >> 4))

#define DTD_VSYNC_H(v) (v & 0x3)
#define DTD_VSYNC_L(v) (v & 0xF)

#define DTD_HI_H(h) ((h & (0xF << 4)) >> 4)
#define DTD_VI_H(h) (h & 0xF)

#define DTD_STEREO_MODE(s) ((s & (0x3 << 6)) >> 6)

/* Parse CEA extension blocks */
#define CEA_EXT_BLK_TAG(b) ((b & (0x7 << 5)) >> 5)
#define CEA_EXT_BLK_DATA_LEN(b) (b & 0x1F)

/* Utility macro to set a vic in a vicdb */
#define SET_VIC(vdb, v) (vdb |= (1 << (v-1)))

/* Utility to check if a bit is set in a value */
#define CHECK_BIT(x, bit) ((x & (1 << bit)) >> bit)

/* Simple yes or no */
#define YESNO(a) (a ? "Yes" : "No")

/* Logs */
#define edid_error printf
#if VERBOSE
#define edid_warn printf
#define edid_debug printf
#else
int edid_warn(const char *format, ...) { return 0;}
int edid_debug(const char *format, ...) { return 0;}
#endif

static inline void
_set_vic(u_int64_t *vicdb, u_int8_t vic)
{
        edid_debug("Setting VIC %d as vic[%d][%d]\n", vic, vic/64, vic%64);
        SET_VIC(vicdb[vic/64], vic%64);
}

static u_int8_t
set_vic(u_int64_t *vicdb, u_int64_t vic)
{
        if ((vic >= 1 && vic <= 127) || (vic >= 193 && vic <= 253)) {
                _set_vic(vicdb, vic);
        } else if (vic >= 129 || vic <= 192) {
                edid_debug("VIC %ld is native mode\n", vic & 0x7F);
                _set_vic(vicdb, vic & 0x7F);
                return vic & 0x7F;
        } else {
                edid_debug("Ignoring invalid vic %ld\n", vic);
        }

        return 0;
}

u_int8_t
*find_data_block_in_edid(struct edid_info *info, u_int8_t *vdblen, enum cea_data_block_tags tag)
{
        u_int8_t blk = 0;
        struct edid_tags *etags = &info->cea_blks;

        for (blk = 1; blk <= etags->n_cea_ext_blks; blk++) {
                u_int8_t *cea_ext = &(info->raw_edid[blk * CEA_EXTN_BLK_SIZE]);
                u_int8_t d = cea_ext[2];
                u_int8_t count = 4;

                if (d <= 4) {
                        edid_debug("Skipping too small CEA block\n");
                        continue;
                }

                /* CEA data block collection is between bytes 4 and d */
                while (count < d) {
                        u_int8_t *db = &cea_ext[count];
                        u_int8_t dblen = CEA_EXT_BLK_DATA_LEN(db[0]);
                        enum cea_data_block_tags blk_tag = CEA_EXT_BLK_TAG(db[0]);

                        if (blk_tag == tag) {
                                edid_debug("Found desired data block in CEA EXT (%d)\n", blk);
                                *vdblen = dblen;
                                return db;
                        }
                        count += dblen;
                }
        }

        edid_debug("No such data block Found in EDID\n");
        return NULL;
}

static inline u_int8_t
find_vic_from_pos(u_int8_t *db, u_int8_t pos)
{
        return db[pos];
}

static void
parse_cea_ext_extended_hdr_dynamic_md_blk(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        u_int8_t *hddb = db;
        u_int8_t hddbl = dblen;
        u_int8_t old_size = 0;

        if (hddbl < 2) {
                edid_warn("Invalid Static HDR MD DB len %d\n", hddbl + 1);
                return;
        }

        /* Todo: The dynamic metadata is being saved in raw form as of now,
         * we can do it better here, and tabulate in a format_tye vs data
         * table */
        if (etags->hdr_dmd.size) {
                /* This is not the first dynamic HDR metadata block */
                old_size = etags->hdr_dmd.size;
                etags->hdr_dmd.data = realloc((void *)etags->hdr_dmd.data,
                                etags->hdr_dmd.size + hddbl);
                etags->hdr_dmd.size += hddbl;

        } else {
                etags->hdr_dmd.data = malloc(hddbl);
                etags->hdr_dmd.size = hddbl;
        }

        memcpy((void *)&(etags->hdr_dmd.data[old_size]), (const void *)hddb, hddbl);
        edid_debug("Found %s dynamic HDR metadata block, size %d\n",
                old_size ? "another" : "", dblen);
}

static void
parse_cea_ext_extended_hdr_static_md_blk(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        u_int8_t *hsdb = db;
        u_int8_t hsdbl = dblen;
        u_int8_t hdr_md;
        float lum;

        if (hsdbl < 2) {
                edid_warn("Invalid Static HDR MD DB len %d\n", hsdbl + 1);
                return;
        }

        hdr_md = hsdb[0];
        etags->hdr_smd.gamma_sdr = CHECK_BIT(hdr_md, CEA_HDR_EOTF_SDR_BIT);
        etags->hdr_smd.gamma_hdr = CHECK_BIT(hdr_md, CEA_HDR_EOTF_HDR_BIT);
        etags->hdr_smd.gamma_st2084 = CHECK_BIT(hdr_md, CEA_HDR_EOTF_ST2084_BIT);
        etags->hdr_smd.gamma_hlg = CHECK_BIT(hdr_md, CEA_HDR_EOTF_HLG_BIT);

        hdr_md = hsdb[1];
        etags->hdr_smd.static_md_type_1 = CHECK_BIT(hdr_md, CEA_HDR_SMD_TYPE1_BIT);

        /* Content luminance data */
        if (hsdbl >= 3) {
                lum = hsdb[2];
                /* Luminance value= 50 * 2 ^(CV/32) */
                etags->hdr_smd.content_max_lum = 50 * pow(2, lum/32);
        }

        if (hsdbl >= 4) {
                lum = hsdb[3];
                /* Luminance value= 50 * 2 ^(CV/32) */
                etags->hdr_smd.content_fav_lum = 50 * pow(2, lum/32);
        }

        if (hsdbl >= 5) {
                lum = hsdb[4];
                /* Desired Content Min Luminance = (Desired Content Max Luminance * (CV/255) 2 / 100) */
                etags->hdr_smd.content_min_lum = (etags->hdr_smd.content_max_lum *
                                        pow(lum/255, 2) / 100);
        }

        edid_debug("Static HDR metadata block: %d bytes\n", hsdbl);
        edid_debug("Gamma: SDR:%s HDR:%s ST2084:%s HLG:%s\n",
                YESNO(etags->hdr_smd.gamma_sdr),
                YESNO(etags->hdr_smd.gamma_hdr),
                YESNO(etags->hdr_smd.gamma_st2084),
                YESNO(etags->hdr_smd.gamma_hlg));
        edid_debug("Luminance(cd/m2): Max:%2f Min:%2f FAV:%2f\n",
                etags->hdr_smd.content_max_lum,
                etags->hdr_smd.content_min_lum,
                etags->hdr_smd.content_fav_lum);
}

static void
parse_cea_ext_extended_vfpdb_blk(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        u_int8_t *vfpdb = db;
        u_int8_t vfpdbl = dblen;
        u_int8_t count;

        if (!vfpdbl) {
                edid_warn("Invalid VFPDB len %d\n", vfpdbl);
                return;
        }

        /* Clamping to 10 SVDs only */
        if (vfpdbl > 10)
                vfpdbl = 10;

        for (count = 0; count < vfpdbl; count++) {
                u_int8_t vic = vfpdb[count];

                if ((vic > 0 && vic < 128) || (vic > 192 && vic < 254))
                        etags->vics_preferred[count] = vic;
                else if (vic >= 128 && vic <= 144)
                        /* todo: convert VIC to DTD */
                        edid_warn("Not handling VIC %d in pref block for now\n", vic);
                else
                        edid_warn("Invalid VIC %d listed in pref block\n", vic);
        }

        edid_debug("Found %d modes in video preference block\n", count);
}

static void
parse_cea_ext_extended_ycbcr420_cmdb_blk(struct edid_info *info, u_int8_t *db, u_int8_t dblen)
{
        u_int8_t vdbl;
        u_int8_t count;
        u_int8_t total;
        u_int8_t vic_index;
        u_int8_t *vdb;
        u_int8_t *cmdb = db;
        u_int8_t cmdbl = dblen;
        struct edid_tags *etags = &info->cea_blks;

        if (!cmdbl) {
                edid_warn("Invalid 4:2:0 CMDB len %d\n", cmdbl);
                return;
        }

        /* CMDB block is tricky, it indicates that which SVDs in Normal VDB
         * blocks can support YCBCR 4:2:0 output also. The translation here
         * is like: 
         * if bit 0 of a cmdb data byte is set, first listed VIC in VDB block supports 4:2:0 also
         * if bit 1 of a cmdb data byte is set, second listed VIC in VDB block supports 4:2:0 also
         * and so on,
         * This means we need VDB block also here
         */

        vdb = find_data_block_in_edid(info, &vdbl, CEA_DATA_BLOCK_VIDEO);
        if (!vdb || !vdbl) {
                edid_error("420 CMDB set, but no VDB found\n");
                return;
        }

        /* First byte in VDB is tag */
        vdb +=1;
        total = 0;
        vic_index = 0;

        for (count = 0; count < cmdbl; count++) {
                u_int8_t byte = cmdb[count];
                u_int8_t pos = 0;

                for (pos = 0; pos < 8; pos++) {
                        u_int8_t vic;

                        if (byte & (1 << pos)) {
                                vic = find_vic_from_pos(vdb, vic_index);
                                set_vic(etags->vics_420_also, vic);
                                edid_debug("VIC %d(index %d) is 420_also mode\n", vic, vic_index);
                                total++;
                        }

                        vic_index++;
                }
        }

        edid_debug("Found total %d YCBCR420_also modes\n", total);

}

static void
parse_cea_ext_extended_ycbcr420_vdb_blk(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        u_int8_t *vdb = db;
        u_int8_t vdbl = dblen;
        u_int8_t count;

        if (!vdbl) {
                edid_warn("Invalid 4:2:0 VDB len %d\n", vdbl);
                return;
        }

        /* This block represents the video modes (VICs) which can be
         * supported only in YCBCR 4:2:0 sampling mode (not RGB/YCBCR444/YCBCR422)
         * etc. Lets call these 420_only modes
         */
        for (count = 0; count < vdbl; count++) {
                u_int8_t vic = vdb[count];

                if (vic) {
                        vic = set_vic(etags->vics_420_only, vic);
                        if (vic)
                        /* Found a native 4:2:0 vic */
                                etags->native_vic = vic;
                }
        }

        edid_debug("Found %d 4:2:0 only modes in YCBCR420_VDB\n", count);
}

static void
parse_cea_ext_extended_ifdb_blk(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        u_int8_t *ifdb = db;
        u_int8_t ifdbl = dblen;

        if (!ifdbl) {
                edid_warn("Invalid IFDB len %d\n", ifdbl + 1);
                return;
        }

        etags->ifdb.data_len = ifdb[0] & 0xE0;
        etags->ifdb.num_vsif = ifdb[1];

        etags->ifdb.data = malloc(etags->ifdb.data_len);
        memcpy((void *)etags->ifdb.data, (const void *)&(ifdb[2]), etags->ifdb.data_len);
        edid_debug("Found EXT IFDB, data len %d, num_vsif %d\n",
                etags->ifdb.data_len,
                etags->ifdb.num_vsif);
}

static void
parse_cea_ext_extended_vsvdb_blk(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        u_int8_t *vsvdb = db;
        u_int8_t vsvdbl = dblen;

        if (vsvdbl < 4) {
                edid_warn("Invalid VSVDB len %d\n", vsvdbl + 1);
                return;
        }

        etags->vsvdb.oui = (vsvdb[2] << 16 | vsvdb[1] << 8 | vsvdb[0]);
        etags->vsvdb.data = malloc(vsvdbl - 3);
        memcpy((void *) etags->vsvdb.data, (const void *)&vsvdb[3], vsvdbl - 3);
        edid_debug("Found EXT VSVDB, len %d, vendor id 0x%x\n",
                vsvdbl + 1,
                etags->vsvdb.oui);
}

static void
parse_cea_ext_extended_vcapability_blk(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        u_int8_t vcap_info;

        if (dblen != 1) {
                edid_warn("Ignoring video capabiliity block with invalid length %d\n", dblen);
                return;
        }

        vcap_info = db[0];
        etags->vcap.quant_range_selectable_ycc = CHECK_BIT(vcap_info, CEA_VCAP_QUANT_RANGE_SELECT_YCC_BIT);
        etags->vcap.quant_range_selectable_rgb = CHECK_BIT(vcap_info, CEA_VCAP_QUANT_RANGE_SELECT_RGB_BIT);
        etags->vcap.pt = CEA_VCAP_PT_SCANNING_VAL(vcap_info);
        etags->vcap.it = CEA_VCAP_IT_SCANNING_VAL(vcap_info);
        etags->vcap.ce = CEA_VCAP_CE_SCANNING_VAL(vcap_info);

        edid_debug("Vacap info: QS_YCC=%d QS_RGB=%d PT=%d IT=%d CE=%d\n",
                etags->vcap.quant_range_selectable_ycc,
                etags->vcap.quant_range_selectable_rgb,
                etags->vcap.pt,
                etags->vcap.it,
                etags->vcap.ce);
}

static void
parse_cea_ext_extended_colorimetry_blk(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        u_int8_t clr_info;

        if (dblen != 2) {
                edid_warn("Ignoring colorimetry block with invalid length %d\n", dblen);
                return;
        }

        clr_info = db[0];
        etags->colorimetry.BT2020_RGB = CHECK_BIT(clr_info, CEA_EXT_CLR_BT2020RGB_BIT);
        etags->colorimetry.BT2020_YCC = CHECK_BIT(clr_info, CEA_EXT_CLR_BT2020YCC_BIT);
        etags->colorimetry.BT2020_CYCC = CHECK_BIT(clr_info, CEA_EXT_CLR_BT2020CYCC_BIT);
        etags->colorimetry.opRGB = CHECK_BIT(clr_info, CEA_EXT_CLR_opRGB_BIT);
        etags->colorimetry.opYCC_601 = CHECK_BIT(clr_info, CEA_EXT_CLR_opYCC601_BIT);
        etags->colorimetry.sYCC_601 = CHECK_BIT(clr_info, CEA_EXT_CLR_sYCC601_BIT);
        etags->colorimetry.xvYCC_709 = CHECK_BIT(clr_info, CEA_EXT_CLR_xvYCC709_BIT);
        etags->colorimetry.xvYCC_601 = CHECK_BIT(clr_info, CEA_EXT_CLR_xvYCC601_BIT);

        clr_info = db[1];
        etags->colorimetry.DCIP3 = CHECK_BIT(clr_info, CEA_EXT_CLR_DCIP3_BIT);

        edid_debug("Colorimetry values: BT2020 RGB:%d YCC %d CYCC %d\n",
                etags->colorimetry.BT2020_RGB,
                etags->colorimetry.BT2020_YCC,
                etags->colorimetry.BT2020_CYCC);
        edid_debug("opRGB %d, opYCC %d sYCC %d xvYCC_709 %d xvYCC_601 %d\n",
                etags->colorimetry.opRGB,
                etags->colorimetry.opYCC_601,
                etags->colorimetry.sYCC_601,
                etags->colorimetry.xvYCC_709,
                etags->colorimetry.xvYCC_601);
}

static void parse_cea_ext_extended_block(struct edid_info *info, u_int8_t *edb, u_int8_t dblen)
{
        /* First byte is extended tag, and its counted in dblen */
        u_int8_t extag = edb[0];
        u_int8_t *db = &edb[1];
        u_int8_t dbl = dblen -1;
        struct edid_tags *etags = &info->cea_blks;

        edid_debug("CEA Extended DATA BLOCK Type: %s\n", cea_extended_tag_names[extag]);

        /* Todo: Handling only limited blocks now, that too video */
        switch (extag) {
        /* Video Capability Data Block */
        case CEA_DATA_BLOCK_EXT_VCDB:
                parse_cea_ext_extended_vcapability_blk(etags, db, dbl);
                break;

        /* Vendor-Specific Video Data Block */
        case CEA_DATA_BLOCK_EXT_VSVDB:
                parse_cea_ext_extended_vsvdb_blk(etags, db, dbl);
                break;

        /* Colorimetry Data Block */
        case CEA_DATA_BLOCK_EXT_COLORIMETRY:
                parse_cea_ext_extended_colorimetry_blk(etags, db, dbl);
                break;
 
        /* HDR Static Metadata Data Block */
        case CEA_DATA_BLOCK_EXT_HDR_STATIC_MD:
                parse_cea_ext_extended_hdr_static_md_blk(etags, db, dbl);
                break;

        /* HDR Dynamic Metadata Data Block */
        case CEA_DATA_BLOCK_EXT_HDR_DYNAMIC_MD:
                parse_cea_ext_extended_hdr_dynamic_md_blk(etags, db, dbl);
                break;

        /* Video Format Preference Data Block */
        case CEA_DATA_BLOCK_EXT_VF_PREF:
                parse_cea_ext_extended_vfpdb_blk(etags, db, dbl);
                break;
 
        /* YCBCR 4:2:0 Video Data Block */
        case CEA_DATA_BLOCK_EXT_YCBCR420_VDB:
                parse_cea_ext_extended_ycbcr420_vdb_blk(etags, db, dbl);
                break;

        /* YCBCR 4:2:0 Capability Map Data Block */
        case CEA_DATA_BLOCK_EXT_YCBCR420_CMDB:
                parse_cea_ext_extended_ycbcr420_cmdb_blk(info, db, dbl);
                break;

        /* Infoframe data block */
        case CEA_DATA_BLOCK_EXT_IFDB:
                parse_cea_ext_extended_ifdb_blk(etags, db, dbl);
                break;

        /* VESA Display Device Data Block */
        case CEA_DATA_BLOCK_EXT_VDDDB:
                /* Todo: This is a separate spec, no details found in CEA-861-G,
                 * so can't process this as of now */

        /*VESA Video Timing Block Extension */
        case CEA_DATA_BLOCK_EXT_VVTBE:
                /* Todo: This is a separate spec, no details found in CEA-861-G,
                 * so can't process this as of now */

        default:
                edid_warn("Not handling extended tag 0x%x\n", extag);
                return;
        }
}

static void parse_cea_ext_vesa_block(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        /* Todo: Parse VESA timing block */
        edid_warn("Vesa timing block parsing is not yet supported\n");
}

static void parse_cea_ext_speaker_block(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        /* Todo: Parse speaker block */
        edid_warn("Speaker block parsing is not yet supported\n");
}

static void parse_cea_ext_audio_block(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        /* Todo: Parse audio block */
        edid_warn("Audio block parsing is not yet supported\n");
}

static void parse_hdmi_hf_vsdb(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        if (dblen < 4) {
                edid_warn("Invalid hf-vsdb data length %d\n", dblen);
                return;
        }

        etags->hfvsdb.version = db[0];
        etags->hfvsdb.max_tmds_rate_mhz = db[1] * 5;

        etags->hfvsdb.scdc = CHECK_BIT(db[2], HDVSDB_SCDC_BIT);
        etags->hfvsdb.rr = CHECK_BIT(db[2], HDVSDB_RR_BIT);
        etags->hfvsdb.scrambling_340mhz = CHECK_BIT(db[2], HDVSDB_SCRAMBLING_AT_340_BIT);
        etags->hfvsdb.iv = CHECK_BIT(db[2], HDVSDB_IV_BIT);
        etags->hfvsdb.dv = CHECK_BIT(db[2], HDVSDB_DV_BIT);
        etags->hfvsdb.osd_3d = CHECK_BIT(db[2], HDVSDB_3D_OSD_BIT);

        etags->hfvsdb.dc_48_420 = CHECK_BIT(db[3], HDVSDB_DC_420_16BPC);
        etags->hfvsdb.dc_36_420 = CHECK_BIT(db[3], HDVSDB_DC_420_12BPC);
        etags->hfvsdb.dc_30_420 = CHECK_BIT(db[3], HDVSDB_DC_420_10BPC);

        edid_debug("HF-VSDB: Max TMDS clock:%d Mhz, SCDC:%s Scrambling at lower clocks:%s\n",
                etags->hfvsdb.max_tmds_rate_mhz,
                YESNO(etags->hfvsdb.scdc),
                YESNO(etags->hfvsdb.scrambling_340mhz));
        
        edid_debug("4:2:0 deep color 16 BPC:%s 12 BPC:%s 10 BPC:%s\n",
                YESNO(etags->hfvsdb.dc_48_420),
                YESNO(etags->hfvsdb.dc_36_420),
                YESNO(etags->hfvsdb.dc_30_420));
}

static void parse_hdmi_vsdb(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        if (dblen < 2) {
                edid_warn("Ignoring hdmi VSDB length %d < 2\n", dblen);
                return;
        }

        etags->hdmi_vsdb.phy.b = db[0] & 0x0F;
        etags->hdmi_vsdb.phy.a = ((db[0] & 0xF0) >> 4);
        etags->hdmi_vsdb.phy.d = db[1] & 0xF;
        etags->hdmi_vsdb.phy.c = ((db[1] & 0xF0) >> 4);
        edid_debug("HDMI-VSDB: Phy: %d.%d.%d.%d\n",
                etags->hdmi_vsdb.phy.a,
                etags->hdmi_vsdb.phy.b,
                etags->hdmi_vsdb.phy.c,
                etags->hdmi_vsdb.phy.d);

        if (dblen < 3)
                return;

        etags->hdmi_vsdb.ai = CHECK_BIT(db[2], HDVSDB_AI_BIT);
        etags->hdmi_vsdb.dc_48_bpc = CHECK_BIT(db[2], HDVSDB_DC_16BPC);
        etags->hdmi_vsdb.dc_36_bpc = CHECK_BIT(db[2], HDVSDB_DC_12BPC);
        etags->hdmi_vsdb.dc_30_bpc = CHECK_BIT(db[2], HDVSDB_DC_10BPC);
        etags->hdmi_vsdb.dc_ycbcr444 = CHECK_BIT(db[2], HDVSDB_DC_YCBCR444);
        etags->hdmi_vsdb.dvi_dual = CHECK_BIT(db[2], HDVSDB_DVI_DUAL_BIT);
        if (dblen < 4)
                return;

        /* Max TMDS rate in Mhz */
        etags->hdmi_vsdb.max_tmds_clock_mhz = db[3] * 5;

        edid_debug("HDMI-VSDB: Max TMDS clock:%d Mhz, AI:%s DVI Dual:%s\n",
                etags->hdmi_vsdb.max_tmds_clock_mhz,
                YESNO(etags->hdmi_vsdb.ai),
                YESNO(etags->hdmi_vsdb.dvi_dual));

        edid_debug("Deep color 16BPC:%s 12BPC:%s 10BPC:%s\n",
                YESNO(etags->hdmi_vsdb.dc_48_bpc),
                YESNO(etags->hdmi_vsdb.dc_36_bpc),
                YESNO(etags->hdmi_vsdb.dc_30_bpc));
}

static void parse_cea_ext_vendor_block(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        u_int8_t *vsdb = db;
        u_int8_t vsdbl = dblen;

        if (vsdbl < 4) {
                edid_warn("Invalid VSDB len %d\n", vsdbl + 1);
                return;
        }

        etags->vsdb.oui = vsdb[2] << 16 | vsdb[1] << 8 | vsdb[0];

        if (etags->vsdb.oui == HDMI_IEEE_OUI) {
                parse_hdmi_vsdb(etags, &vsdb[3], vsdbl - 3);
                return;
        }

        if (etags->vsdb.oui == HDMI_FORUM_IEEE_OUI) {
                parse_hdmi_hf_vsdb(etags, &vsdb[3], vsdbl - 3);
                return;
        }

        etags->vsdb.datalen = vsdbl - 3;
        etags->vsdb.data = malloc(vsdbl - 3);
        memcpy((void *) etags->vsdb.data, (const void *)&vsdb[3], vsdbl - 3);
        edid_debug("Found VSDB, len %d, vendor oui 0x%x data %d\n",
                vsdbl,
                etags->vsdb.oui,
                etags->vsdb.datalen);
}

static void
parse_cea_ext_video_block(struct edid_tags *etags, u_int8_t *db, u_int8_t dblen)
{
        u_int8_t *vdb = db;
        u_int8_t vdbl = dblen;
        u_int8_t count;

        edid_debug("Parsing video block, dblen %d\n", dblen);
        for (count = 0; count < vdbl; count++) {
                uint8_t vic = vdb[count];

                if (vic) {
                        vic = set_vic(etags->vics, vic);
                        if (vic)
                                /* Found native mode */
                                etags->native_vic = vic;
                }
        }
}

static void
parse_cea_data_block(struct edid_info *info, u_int8_t *db, u_int8_t dblen)
{
        enum cea_data_block_tags tag;
        struct edid_tags *etags = &info->cea_blks;

        tag = CEA_EXT_BLK_TAG(db[0]);
        if (!dblen) {
                edid_error("Skipping invalid sized(%d) block tag %d\n", dblen, tag);
                return;
        }

        switch (tag) {
        case CEA_DATA_BLOCK_VIDEO:
                parse_cea_ext_video_block(etags, db + 1, dblen);
                break;

        case CEA_DATA_BLOCK_VENDOR:
                parse_cea_ext_vendor_block(etags, db + 1, dblen);
                break;

        case CEA_DATA_BLOCK_EXTENDED:
                parse_cea_ext_extended_block(info, db + 1, dblen);
                break;

        case CEA_DATA_BLOCK_AUDIO:
                /* Todo: Support audio data processing */
                parse_cea_ext_audio_block(etags, db + 1, dblen);
                break;

        case CEA_DATA_BLOCK_SPEAKER_ALLOC:
                /* Todo: Support speaker data processing */
                parse_cea_ext_speaker_block(etags, db + 1, dblen);
                break;

        case CEA_DATA_BLOCK_VESA_DTC:
                /* Todo: This is a diff spec, not handling this block now */
                parse_cea_ext_vesa_block(etags, db + 1, dblen);
                break;

        case CEA_DATA_BLOCK_RESERVED_2:
        case CEA_DATA_BLOCK_RESERVED:
        default:
                edid_warn("Invalid data block type %d\n", tag);
        }
}

static void extract_dtd_mode(u_int8_t *db, struct detailed_mode *mode)
{
        memset(mode, 0, sizeof(struct detailed_mode));
        /* Stored Value = Pixel clock ÷ 10,000 Hz */
        mode->pixel_clock_khz = (db[1] << 8 | db[0]) * 10;
        if (!mode->pixel_clock_khz)
                return;

        mode->hactive = (DTD_ACT(db[4]) << 8) | db[2];
        mode->hblank = (DTD_BL(db[4]) << 8) | db[3];

        mode->vactive = (DTD_ACT(db[7]) << 8) | db[5];
        mode->vblank = (DTD_BL(db[7]) << 8) | db[6];

        mode->hfrontp = (DTD_FP(db[11]) << 8)| db[8];
        mode->hsync = (DTD_HSYNC(db[11]) << 8) | db[9];

        mode->vfrontp = DTD_VFP_H(db[11]) << 8 | DTD_VFP_L(db[10]);
        mode->vsync = DTD_VSYNC_H(db[11]) << 8 | DTD_VSYNC_L(db[10]);

        mode->hsize_mm = DTD_HI_H(db[14]) << 8 | db[12];
        mode->vsize_mm = DTD_VI_H(db[14]) << 8 | db[13];

        mode->hborder_2 = db[15] * 2;
        mode->vborder_2 = db[16] * 2;

        mode->stereo = CHECK_BIT(db[17], 0) ? (DTD_STEREO_MODE(db[17])): 0;
        mode->interlaced = CHECK_BIT(db[17], 7);

        edid_debug("\nDetailed mode: %dx%d clock:%d Khz\n",
                mode->hactive,
                mode->vactive,
                mode->pixel_clock_khz);
        edid_debug("3D:%s Interlaced:%s\n",
                YESNO(mode->stereo),
                YESNO(mode->interlaced));
        edid_debug("HA:%d HBL:%d HFP:%d HSYNC:%d HSZ:%d HB:%d\n",
                mode->hactive,
                mode->hblank,
                mode->hfrontp,
                mode->hsync,
                mode->hsize_mm,
                mode->hborder_2);
        edid_debug("VA:%d VBL:%d VFP:%d VSYNC:%d VSZ:%d VB:%d\n",
                mode->vactive,
                mode->vblank,
                mode->vfrontp,
                mode->vsync,
                mode->vsize_mm,
                mode->vborder_2);
}

static void parse_cea_dtd_block(struct edid_tags *etags, u_int8_t *db)
{
        u_int8_t count;
        u_int8_t blk_sz;
        struct dtd_blk *dtdb = &etags->dtd;

        if (!etags->n_dtd_blks) {
                edid_debug("No DTD found in CEA Extension block\n");
                return;
        }

        dtdb->n_dtd_modes = etags->n_dtd_blks;
        blk_sz = etags->n_dtd_blks * sizeof(struct detailed_mode);
        dtdb->d_modes = malloc(blk_sz);

        edid_debug("\n================================\n");
        edid_debug("Found %d DTD modes\n", dtdb->n_dtd_modes);
        for (count = 0; count < dtdb->n_dtd_modes; count++)
                extract_dtd_mode(&db[count * 18], &dtdb->d_modes[count]);
        edid_debug("================================\n");
        return;
}

void extract_cea_block_information(struct edid_info *info, u_int8_t *cea)
{
        u_int8_t tag = cea[0];
        u_int8_t d;
        u_int8_t count = 0;
        struct edid_tags *etags = &info->cea_blks;

        if (tag != CEA_EXT_BLK_TAG_VALUE) {
                edid_error("Invalid CEA block tag %d\n", tag);
                return;
        }

        d = cea[2];
        if (d < 4) {
                edid_warn("Empty/Bad CEA extenstion block, d=%d\n", d);
                return;
        }

        etags->it_underscan = CHECK_BIT(cea[3], CEA_EXT_IT_UNDESCAN_BIT);
        etags->audio = CHECK_BIT(cea[3], CEA_EXT_AUDIO_BIT);
        etags->ycbcr444 = CHECK_BIT(cea[3], CEA_EXT_YCBCR444_BIT);
        etags->ycbcr422 = CHECK_BIT(cea[3], CEA_EXT_YCBCR422_BIT);
        etags->n_dtd_blks = cea[3] & 0xF;

        edid_debug("\n##############################\n");
        edid_debug("##### CEA Exension block #####\n");
        edid_debug("##############################\n");
        edid_debug("Basic information of CEA extn block: d %d underscan %s audio %s\n",
                d,
                YESNO(etags->it_underscan),
                YESNO(etags->audio));
        
        edid_debug("ycbcr444 %s 422 %s dtd blocks %d\n",
                YESNO(etags->ycbcr444),
                YESNO(etags->ycbcr422),
                etags->n_dtd_blks);

        edid_debug("CEA data block collection size = %d bytes\n", d - 4);

        /* Parse CEA Data Block Collection */
        if ( d != 4) {
                u_int8_t start = 4;
                u_int8_t blk = 0;

                /* Data blocks are between bytes 4 and d */
                while (start < d) {
                        u_int8_t *db = &cea[start];
                        u_int8_t dblen = CEA_EXT_BLK_DATA_LEN(cea[start]);
                        u_int8_t tag = CEA_EXT_BLK_TAG(cea[start]);

                        if (!dblen) {
                                edid_warn("Invalid dblen and/or tag %d/%d, skipping parsing data blocks\n",
                                dblen, CEA_EXT_BLK_TAG(tag));
                        }

                        edid_debug("\n================================\n");
                        edid_debug("CEA DATA BLOCK (%d) Type: %s\n", ++blk, cea_db_names[tag]);
                        edid_debug("Bytes %d - %d (%d bytes + tag)\n", start, start + dblen, dblen);
                        edid_debug("=================================\n");
                        parse_cea_data_block(info, db, dblen);

                        /* The dblen doesn't include tag byte, so +1 */
                        start +=  (dblen + 1);
                }
        }

        /* Parse detailed timing descriptor blocks */
        for (count = 0; count < etags->n_dtd_blks; count++)
                parse_cea_dtd_block(etags, &cea[count * 18 + d]);
}

static int
process_edid_cea_extension_blocks(u_int8_t *raw_edid, struct edid_info *info)
{
        int count;
        struct edid *edid_first_blk = (struct edid *)raw_edid;
        struct edid_tags *etags = &info->cea_blks;

        etags->n_cea_ext_blks = edid_first_blk->extensions;
        if (!etags->n_cea_ext_blks) {
                edid_debug("No CEA-861 extension blocks in EDID\n");
                return 0;
        }

        edid_debug("Found %d CEA extension blocks in EDID\n", etags->n_cea_ext_blks);
        for (count = 1; count <= etags->n_cea_ext_blks; count++) {
                u_int8_t *cea_extn = &(raw_edid[count * CEA_EXTN_BLK_SIZE]);

                extract_cea_block_information(info, cea_extn);
        }

        return 0;
}

static void
edid_bb_get_input_details(struct edid *edid, struct edid_base_blk *bb)
{
        uint8_t input = edid->input;

        /* Bothering only about digital inputs */
        if (input & EDID_INPUT_IS_DIGITAL) {

                input = EDID_INPUT_CLR_DEPTH(input);
                switch (input) {
                case CLR_DEPTH_6BPC:
                        bb->clr_depth = 6;
                        break;
                case CLR_DEPTH_8BPC:
                        bb->clr_depth = 8;
                        break;
                case CLR_DEPTH_10BPC:
                        bb->clr_depth = 10;
                        break;
                case CLR_DEPTH_12BPC:
                        bb->clr_depth = 12;
                        break;
                case CLR_DEPTH_14BPC:
                        bb->clr_depth = 14;
                        break;
                case CLR_DEPTH_16BPC:
                        bb->clr_depth = 16;
                        break;
                default: 
                        edid_debug("No color depth(%d) specified by monitor\n", input);
                        goto next;
                }

                edid_debug("EDID color depth is (%d)\n", bb->clr_depth);

next:
                input = EDID_INPUT_CLR_FORMAT(edid->features);

                bb->clr_formats.clr_format_ycbcr444 = CHECK_BIT(input, EDID_CLR_FORMAT_YCBCR_444_BIT);
                bb->clr_formats.clr_format_ycbcr422 = CHECK_BIT(input, EDID_CLR_FORMAT_YCBCR_422_BIT);
                bb->clr_formats.clr_format_ycbcr420 = CHECK_BIT(input, EDID_CLR_FORMAT_YCBCR_420_BIT);
                bb->clr_formats.clr_format_rgb444 = 1;

                edid_debug("EDID output formats:\n");
                edid_debug("RGB(%d) YCBCR444(%d) YCBCR422(%d), YCBCR420(%d)\n",
                        bb->clr_formats.clr_format_rgb444,
                        bb->clr_formats.clr_format_ycbcr444,
                        bb->clr_formats.clr_format_ycbcr422,
                        bb->clr_formats.clr_format_ycbcr420);

                bb->srgb_default = EDID_SRGB_CLRSP_DEFAULT(edid->features);
                edid_debug("EDID output colorspace SRGB is default: %s\n",
                        YESNO(bb->srgb_default));

                bb->standby = EDID_STANDBY_SUPPORTED(edid->features);
                bb->suspend = EDID_SUSPEND_SUPPORTED(edid->features);
                bb->active_off = EDID_ACTIVE_OFF_SUPPORTED(edid->features);

                edid_debug("EDID features standby(%d) suspend(%d) active off(%d)\n",
                        bb->standby,
                        bb->suspend,
                        bb->active_off);

                /* gamma = (value + 100)/100 */
                bb->gamma = (edid->gamma + 100) / 100;
                edid_debug("EDID gamma %f\n", bb->gamma);
        }
}

static void
edid_bb_get_product_details(struct edid *edid, struct edid_base_blk *bb)
{
        bb->vendor[0] = ((edid->mfg_id[0] & 0x7c) >> 2) + '@';
	bb->vendor[1] = (((edid->mfg_id[0] & 0x3) << 3) | ((edid->mfg_id[1] & 0xe0) >> 5)) + '@';
	bb->vendor[2] = (edid->mfg_id[1] & 0x1f) + '@';
        bb->vendor[3] = '\0';

        bb->pid = ((edid->prod_code[1] << 8) | edid->prod_code[0]);
        bb->sno = edid->serial;
        edid_debug("Product details:\n");
        edid_debug("Vendor(%s), Product code(%d), Serial no(%u)\n",
                bb->vendor, (int)(bb->pid), bb->sno);
}

static void
edid_bb_get_dtd_modes(u_int8_t *raw_edid, struct edid_base_blk *bb)
{
        u_int8_t count;
        u_int8_t *db = &raw_edid[54];

        edid_debug("\nDTD Modes in base blk:\n");
        /* There can be 4 detailed mode blocks, 18 bytes each */
        for (count = 0; count < 4; count++)
                extract_dtd_mode(&db[count * 18], &bb->dmodes[count]);
        edid_debug("\n");
}

int process_edid_base_block(u_int8_t *raw_edid, struct edid_info *info)
{
        const u_int8_t header[] = {0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0};
        struct edid *edid = (struct edid *)raw_edid;
        struct edid_base_blk *bb = &info->base_blk;

        if (!edid) {
                edid_error("No EDID found in input\n");
                return -1;
        }

        if (memcmp(edid->header, header, 8)) {
                edid_error("Corrupt EDID: Header mismatch\n");
                return -1;
        }

        edid_debug("\n##############################\n");
        edid_debug("####### EDID Main block ######\n");
        edid_debug("##############################\n");

        edid_bb_get_product_details(edid, bb);
        edid_bb_get_input_details(edid, bb);
        edid_bb_get_dtd_modes(raw_edid, bb);
        return 0;
}

void libedid_destroy_edid_info(struct edid_info *info)
{
        struct edid_tags *etags;

        if (!info)
                return;

        etags = &info->cea_blks;

        if (etags->ifdb.data) {
                free(etags->ifdb.data);
                etags->ifdb.data = NULL;
        }

        if (etags->hdr_dmd.data) {
                free(etags->hdr_dmd.data);
                etags->hdr_dmd.data = NULL;
        }

        if (etags->vsdb.data) {
                free(etags->vsdb.data);
                etags->vsdb.data = NULL;
        }

        if (etags->vsvdb.data) {
                free(etags->vsvdb.data);
                etags->vsvdb.data = NULL;
        }

        if (etags->dtd.d_modes) {
                free(etags->dtd.d_modes);
                etags->dtd.d_modes = NULL;
        }

        free(info);
}

struct edid_info
*libedid_process_edid_info(u_int8_t *raw_edid)
{
        struct edid_info *info;

        info = malloc(sizeof(struct edid_info));
        if (!info) {
                edid_error("Out ot memory\n");
                return NULL;
        }

        memset(info, 0, sizeof(struct edid_info));
        info->raw_edid = raw_edid;

        if (process_edid_base_block(raw_edid, info)) {
                edid_error("Failed to process base edid blocks\n");
                goto error_free_info;
        }

        if (process_edid_cea_extension_blocks(raw_edid, info)) {
                edid_error("Failed to process CEA extension blocks\n");
                goto error_free_info;
        }

        return info;

error_free_info:
        free(info);
        edid_error("Failed to process EDID\n");
        return NULL;
}