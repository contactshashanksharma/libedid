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


struct edid_info {
        u_int8_t *raw_edid;----------------------> Raw EDID 
        struct edid_tags cea_blks; --------------> information about CEA Extension block video features and configuration (As per CEA-861-G spec)
        struct edid_base_blk base_blk;-----------> information on first edid block (as per VESA E-EDID spec 1.4)
};

There is a lot of documentation provided along with the definitions of these structures, hope it will be self-explanatory.

===================
How to use libedid:
===================
There are only 2 APIs in this library:
	- libedid_process_edid_info: Get EDID information from raw_edid, returns filled struct edid_info ptr
	- libedid_destroy_edid_info: Free the EDID information

There are 2 test files provided here, which explains the right usage of the APIs:
1. test_libedid.c : Test libedid with hard-coded EDIDs
2. test_libedid_drm.c: Test libedid with the EDID extracted from the DRM connectors of a card

=========
Building
=========

make : Builds the library (libedid.so), and two test apps (test-libedid and test-libedid-drm)
make lib: Builds only the library libedid.so
make test: builds only the first test app (test_libedid)
make test-drm: builds only the second test app (test_libedid_drm)

===========
Debug logs
===========
To enable debug logs, change VERBOSE=0 to VERBOSE=1 in the Makefile, or simply "make verbose"
There is a sample output.txt file which shows the sample logs
