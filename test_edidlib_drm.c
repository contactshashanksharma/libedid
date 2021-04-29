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
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <xf86drmMode.h>
#include <unistd.h>
#include <malloc.h>
#include "libedid.h"

#define DRM_CARD_0 "/dev/dri/card0"
#define DRM_CARD_1 "/dev/dri/card1"

static u_int8_t *get_edid_from_prop(int drm_fd, drmModePropertyPtr prop)
{
        u_int32_t blob_id;
        u_int8_t *edid;
        drmModePropertyBlobRes *edid_blob;

        if (!prop->count_blobs) {
                printf("Error: No blob found in prop %s\n", prop->name);
                return NULL;
        }

        blob_id = prop->blob_ids[0];
        edid_blob = drmModeGetPropertyBlob(drm_fd, blob_id);
        if (!edid_blob) {
                printf("Error: Could not get EDID blob (id %d)\n", blob_id);
                return NULL;
        }

        edid = malloc(edid_blob->length);
        memcpy(edid, edid_blob->data, edid_blob->length);
        drmModeFreePropertyBlob(edid_blob);
        return edid;
}

int main (void)
{
        drmModeRes *res;
        int drm_fd = -1, i;

        drm_fd = open(DRM_CARD_0, O_RDWR);
        if (drm_fd < 0) {
                printf("Open failed\n");
                return -1;
        }

        res = drmModeGetResources(drm_fd);
        if (!res) {
                printf("Could not get res\n");
                close(drm_fd);
                return -2;
        }

        for (i = 0; i < res->count_connectors; i++) {
                drmModeConnector *connector;
                drmModeObjectPropertiesPtr props;
                int j;

                connector = drmModeGetConnector(drm_fd, res->connectors[i]);
                if (!connector) {
                        printf("Error: could not get connector %d\n", res->connectors[i]);
                        continue;
                }

                props = drmModeObjectGetProperties(drm_fd,
                                		   connector->connector_id,
                                		   DRM_MODE_OBJECT_CONNECTOR);
                if (!props) {
                        printf("No properties on conn %d\n", connector->connector_id);
                        goto free_conn;
                }

                for (j = 0; j < props->count_props; j++) {
                        drmModePropertyPtr prop = drmModeGetProperty(drm_fd, props->props[j]);
                        u_int8_t *raw_edid;
                        struct edid_info *info;

                        if (!strncmp(prop->name, "EDID", 4)) {
                                printf("Found EDID prop on connector %d\n", connector->connector_id);
                                raw_edid = get_edid_from_prop(drm_fd, prop);
                                if (!raw_edid) {
                                        printf("Error: Could not get edid\n");
                                        goto free_prop;
                                }

                                info = libedid_process_edid_info(raw_edid);
                                if (!info) {
                                        printf("Error: Could not process edid\n");
                                        goto free_prop;
                                }

                                printf("Edid processing done, got info\n");
                                libedid_destroy_edid_info(info);
                                free(raw_edid);
                        }
free_prop:
                        drmModeFreeProperty(prop);
                }

                drmModeFreeObjectProperties(props);
free_conn:
                drmModeFreeConnector(connector);
        }

        drmModeFreeResources(res);
        close(drm_fd);
        return 0;
}
