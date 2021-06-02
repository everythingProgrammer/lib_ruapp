#include <inttypes.h>

/* in and out type can be same */
enum software_pkg_opcode {
    DOWNLOAD_PKG = 0,
    INSTALL_PKG,
    ACTIVATE_PKG,

    FILE_MGMT_UPLOAD
};

typedef struct download_input {
    char *uri;
} download_input_t;

typedef struct download_output {
    int status;
    uint32_t notification_timeout;
    char *error_message;
} download_output_t;

typedef struct install_input {
    char *slot_name;
    char *file_names[];
} install_input_t;

typedef struct install_output {
    int status;
    char *error_message;
} install_output_t;

typedef struct activate_input {
    char *slot_name;
} activate_input_t;

typedef struct activate_output {
    int status;
    uint32_t notification_timeout;
    char *error_message;
} activate_output_t;

typedef struct ru_software_pkg_in{
    uint8_t type;
    union {
         download_input_t downlod_in;
         install_input_t install_in;
         activate_input_t activate_in;
    };
} ru_software_pkg_in_t;

typedef struct ru_software_pkg_out{
    uint8_t type;
    union {
         download_output_t download_out;
         install_input_t install_out;
	 activate_output_t activate_out;
    };
} ru_software_pkg_out_t;

typedef struct file_upload_input {
   char *local_logical_file_path;
   char *remote_file_path; // Format:sftp://<username>@<host>[:port]/path
} file_upload_input_t;

typedef struct file_upload_output {
   int status;
   char *reject_reason;
} file_upload_output_t;


typedef struct file_retrieve_input {
   char *logical_path; // ex :  O-RAN/log, o-RAN/PM, O-RAN/transceiver
   char *file_name_filter;
} file_retrieve_input_t;

typedef struct file_retrieve_output {
   int status;
   char *reject_reason;
   char *file_list;
} file_retrieve_output_t;

typedef struct file_download_input {
   char *local_logical_file_path;
   char *remote_file_path; // Format:sftp://<username>@<host>[:port]/path
} file_download_input_t;

typedef struct file_download_output {
   int status;
   char *reject_reason;
} file_download_output_t;

typedef struct ru_file_mgmt_in{
    uint8_t type;
    union {
         file_upload_input_t file_upload_in;
	 file_retrieve_input_t file_retrieve_in;
	 file_download_input_t file_download_in;
    };
} ru_file_mgmt_in_t;

typedef struct ru_file_mgmt_out{
    uint8_t type;
    union {
         file_upload_output_t file_upload_out;
	 file_retrieve_output_t file_retrieve_out;
	 file_download_output_t file_download_out;
    };
} ru_file_mgmt_out_t;


extern int software_download(ru_software_pkg_in_t *in, ru_software_pkg_out_t **out);
extern int software_install(ru_software_pkg_in_t *in, ru_software_pkg_out_t **out);
extern int software_activate(ru_software_pkg_in_t *in, ru_software_pkg_out_t **out);
extern int reset();
extern int file_upload(ru_file_mgmt_in_t *in, ru_file_mgmt_out_t **out);
extern int retrieve_file_list(ru_file_mgmt_in_t *in, ru_file_mgmt_out_t **out);
extern int file_download(ru_file_mgmt_in_t *in, ru_file_mgmt_out_t **out);

