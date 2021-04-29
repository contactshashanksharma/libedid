/*
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

/*
 * Disclaimer: All of these definitions in this file are copied from
 * Linux kernel's drm_edid.h, so that kernel and userspace can be aligned
 * on the EDID base block handling. Taking this moment to give due credit
 * and respect to the original authors.
 */

#ifndef EDID_TIMING_H
#define EDID_TIMING_H
#include <sys/types.h>

struct est_timings {
	u_int8_t t1;
	u_int8_t t2;
	u_int8_t mfg_rsvd;
} __attribute__((packed));

struct std_timing {
	u_int8_t hsize; /* need to multiply by 8 then add 248 */
	u_int8_t vfreq_aspect;
} __attribute__((packed));

struct detailed_pixel_timing {
	u_int8_t hactive_lo;
	u_int8_t hblank_lo;
	u_int8_t hactive_hblank_hi;
	u_int8_t vactive_lo;
	u_int8_t vblank_lo;
	u_int8_t vactive_vblank_hi;
	u_int8_t hsync_offset_lo;
	u_int8_t hsync_pulse_width_lo;
	u_int8_t vsync_offset_pulse_width_lo;
	u_int8_t hsync_vsync_offset_pulse_width_hi;
	u_int8_t width_mm_lo;
	u_int8_t height_mm_lo;
	u_int8_t width_height_mm_hi;
	u_int8_t hborder;
	u_int8_t vborder;
	u_int8_t misc;
} __attribute__((packed));

struct detailed_data_string {
	u_int8_t str[13];
} __attribute__((packed));

struct detailed_data_monitor_range {
	u_int8_t min_vfreq;
	u_int8_t max_vfreq;
	u_int8_t min_hfreq_khz;
	u_int8_t max_hfreq_khz;
	u_int8_t pixel_clock_mhz; /* need to multiply by 10 */
	u_int8_t flags;
	union {
		struct {
			u_int8_t reserved;
			u_int8_t hfreq_start_khz; /* need to multiply by 2 */
			u_int8_t c; /* need to divide by 2 */
			u_int16_t m;
			u_int8_t k;
			u_int8_t j; /* need to divide by 2 */
		} __attribute__((packed)) gtf2;
		struct {
			u_int8_t version;
			u_int8_t data1; /* high 6 bits: extra clock resolution */
			u_int8_t data2; /* plus low 2 of above: max hactive */
			u_int8_t supported_aspects;
			u_int8_t flags; /* preferred aspect and blanking support */
			u_int8_t supported_scalings;
			u_int8_t preferred_refresh;
		} __attribute__((packed)) cvt;
	} formula;
} __attribute__((packed));

struct detailed_data_wpindex {
	u_int8_t white_yx_lo; /* Lower 2 bits each */
	u_int8_t white_x_hi;
	u_int8_t white_y_hi;
	u_int8_t gamma; /* need to divide by 100 then add 1 */
} __attribute__((packed));

struct cvt_timing {
	u_int8_t code[3];
} __attribute__((packed));

struct detailed_non_pixel {
	u_int8_t pad1;
	u_int8_t type; /* ff=serial, fe=string, fd=monitor range, fc=monitor name
		    fb=color point data, fa=standard timing data,
		    f9=undefined, f8=mfg. reserved */
	u_int8_t pad2;
	union {
		struct detailed_data_string str;
		struct detailed_data_monitor_range range;
		struct detailed_data_wpindex color;
		struct std_timing timings[6];
		struct cvt_timing cvt[4];
	} data;
} __attribute__((packed));

struct detailed_timing {
	u_int16_t pixel_clock; /* need to multiply by 10 KHz */
	union {
		struct detailed_pixel_timing pixel_data;
		struct detailed_non_pixel other_data;
	} data;
} __attribute__((packed));

struct edid {
        u_int8_t header[8];
	/* Vendor & product info */
	u_int8_t mfg_id[2];
	u_int8_t prod_code[2];
	u_int32_t serial; /* FIXME: byte order */
	u_int8_t mfg_week;
	u_int8_t mfg_year;
	/* EDID version */
	u_int8_t version;
	u_int8_t revision;

	/* Display info: */
	u_int8_t input;
	u_int8_t width_cm;
	u_int8_t height_cm;
	u_int8_t gamma;
	u_int8_t features;

	/* Color characteristics */
	u_int8_t red_green_lo;
	u_int8_t black_white_lo;
	u_int8_t red_x;
	u_int8_t red_y;
	u_int8_t green_x;
	u_int8_t green_y;
	u_int8_t blue_x;
	u_int8_t blue_y;
	u_int8_t white_x;
	u_int8_t white_y;

	/* Est. timings and mfg rsvd timings*/
	struct est_timings established_timings;

	/* Standard timings 1-8*/
	struct std_timing standard_timings[8];

	/* Detailing timings 1-4, 18 * 4 = 72 bytes */
	struct detailed_timing detailed_timings[4]; 

	u_int8_t extensions;

	/* Checksum */
	u_int8_t checksum;
} __attribute__((packed));

#endif