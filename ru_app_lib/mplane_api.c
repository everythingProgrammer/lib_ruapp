#include <stdio.h>
#include "mplane_api.h"

int
software_download(ru_software_pkg_in_t *in,
                  ru_software_pkg_out_t **out)
{
    printf("URI: %s\n", in->downlod_in.uri);
    return 0;
}

int software_install(ru_software_pkg_in_t *in,
                     ru_software_pkg_out_t **out)
{
    printf("Install from slot: %s\n", in->install_in.slot_name);
    return 0;
}

int
software_activate(ru_software_pkg_in_t *in,
                  ru_software_pkg_out_t **out)
{
    printf("Slot-name: %s\n", in->activate_in.slot_name);
    return 0;
}

int
file_upload(ru_file_mgmt_in_t *in,
            ru_file_mgmt_out_t **out)
{
    printf("Local PATH: %s\n", in->file_upload_in.local_logical_file_path);
    printf("Remote PATH: %s\n", in->file_upload_in.remote_file_path);
    return 0;
}

int
retrieve_file_list(ru_file_mgmt_in_t *in,
                   ru_file_mgmt_out_t **out)
{
    printf("Logical PATH: %s\n", in->file_retrieve_in.logical_path);
    printf("File Filter: %s\n", in->file_retrieve_in.file_name_filter);
    return 0;
}

int
file_download(ru_file_mgmt_in_t *in,
              ru_file_mgmt_out_t **out)
{
    printf("file-download rpc hit\n");
    printf("Local PATH: %s\n", in->file_download_in.local_logical_file_path);
    printf("Remote PATH: %s\n", in->file_download_in.remote_file_path);
    return 0;
}

