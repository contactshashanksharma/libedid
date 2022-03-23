========
LIBEDID
=========

This is a simple C library to extract information from RAW edid data blob. There are a few tools available which already do this, but most of them focus of the base EDID block(first block). This library provides the advanced display configuration information from the CEA extension blocks as well.

This library is meant to serve the Display compositors. Compositors need to take multiple important decisions about the display configuration, but they are limited due to lack of information from the EDID. This library extracts the video information of the Display, and provides it in form of a data structure struct edid_info;

PS: This librery only extract Video related information, Audio information is not supported yet.

==========
About EDID
===========

/* An EDID is a binary blob, which is kept in the ROM of a display sink (HDMI or DP)
 * EDID contains information about the capabilities of the display sink, and is read
 * by the display driver over DDC I2C channel. Most of the DRM display drivers save
 * EDID in connector->edid property blob.
 *
 * An EDID must contain a base block, and it can contain upto 3 CEA     
 * Extension blocks.
 * 
 * ┌─────────────────────────────────────┐
 * │                                     │
 * │                                     │
 * │                                     │
 * │     EDID Base block 128 bytes       │
 * │                                     │
 * │                                     │
 * ├─────────────────────────────────────┤
 * │                                     │
 * │                                     │
 * │     CEA-Extension block 128 bytes   │
 * │                                     │
 * │                                     │
 * │                                     │
 * ├─────────────────────────────────────┤
 * │                                     │
 * │                                     │
 * │     CEA Extension block 128 bytes   │
 * │                                     │
 * │                                     │
 * │                                     │
 * │                                     │
 * ├─────────────────────────────────────┤
 * │                                     │
 * │     CEA Extension block 128 bytes   │
 * │                                     │
 * │                                     │
 * │                                     │
 * └─────────────────────────────────────┘
 */

There is a lot of documentation provided along with the definitions of these structures, hope it will be self-explanatory.

===================
How to use libedid:
===================

1. Parse EDID using function libedid_init() by passing raw EDID data;
   This function rerutns a void ptr. Keep it safe.
2. Check the available APIs in libedid-api.h, call any of those APIs with the returned pointer to
   extract a specific information. For example, libedid_display_supports_hdr_output() will tell
   if the display supports HDR.

   Checkout the sample program (test-libedid-api.c) for example code and details.
3. Free the memory using libedid_destroy() function when done with this display.

=========
Building
=========

make : Builds the library (libedid.so), and test apps (test-libedid and test-libedid-drm)
make lib: Builds only the library libedid.so
make test: builds only the first test app (test_libedid)
make test-drm: builds only the second test app (test_libedid_drm)
make test-api: builds only the example test app, which demos the API usage (test-api)
make verbose: build all of those above with debug prints and flags enabled

===========
Debug logs
===========
To enable debug logs, change VERBOSE=0 to VERBOSE=1 in the Makefile, or simply "make verbose"
There is a sample output.txt file which shows the sample logs
