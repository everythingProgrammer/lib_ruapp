#include <string.h>
#include "sysrepo.h"
#include <dlfcn.h>

#define MAX_MOD_NAME_LEN 128
#define MAX_BUFF_SIZE 1024

#define O_RAN_RPC_SUBSCR(xpath, cb) \
    rc = sr_rpc_subscribe(oran_srv.sr_sess, xpath, cb, NULL, 0, SR_SUBSCR_CTX_REUSE, &oran_srv.sr_rpc_sub); \
    if (rc != SR_ERR_OK) { \
        printf("Subscribing for \"%s\" RPC failed (%s).", xpath, sr_strerror(rc)); \
        goto error; \
    }

extern void
print_val(const sr_val_t *value);

extern void
print_change(sr_change_oper_t op,
              sr_val_t *old_val,
              sr_val_t *new_va);

extern const
char * ev_to_str(sr_event_t ev);

extern void
process_module_change(sr_change_oper_t oper,
                      sr_val_t *value);

extern int
module_change_cb(sr_session_ctx_t *session,
                 const char *module_name,
                 const char *xpath,
                 sr_event_t event,
                 uint32_t request_id,
                 void *private_data);
extern int
mplane_oran_software_install(sr_session_ctx_t *session,
                             const char *path,
                             const sr_val_t *input,
                             const size_t input_cnt,
                             sr_event_t event,
                             uint32_t request_id,
                             sr_val_t **output,
                             size_t *output_cnt,
                             void *private_data);

extern int
subscribe_o_ran_rpcs(char *xpath,
                       int(*cb)());

extern int
module_change_subscribe();

extern int
init_sysrepo();

extern char
*read_oper_data_file();

extern int
sw_inventory_get_items_cb(sr_session_ctx_t *session,
                          const char *module_name,
                          const char *xpath,
                          const char *request_xpath,
                          uint32_t request_id,
                          struct lyd_node **parent,
                          void *private_data);

extern int
subscribe_operation_data();

extern int
o_ran_lib_init();

extern int
o_ran_lib_deinit();

extern int
mplane_oran_software_activate(sr_session_ctx_t *session,
                              const char *path,
                              const sr_val_t *input,
                              const size_t input_cnt,
                              sr_event_t event,
                              uint32_t request_id,
                              sr_val_t **output,
                              size_t *output_cnt,
                              void *private_data);

extern int
mplane_oran_file_upload(sr_session_ctx_t *session,
                        const char *path,
                        const sr_val_t *input,
                        const size_t input_cnt,
                        sr_event_t event,
                        uint32_t request_id,
                        sr_val_t **output,
                        size_t *output_cnt,
                        void *private_data);

extern int
mplane_oran_retrieve_file_list(sr_session_ctx_t *session,
                               const char *path,
                               const sr_val_t *input,
                               const size_t input_cnt,
                               sr_event_t event,
                               uint32_t request_id,
                               sr_val_t **output,
                               size_t *output_cnt,
                               void *private_data);

extern int
mplane_oran_file_download(sr_session_ctx_t *session,
                          const char *path,
                          const sr_val_t *input,
                          const size_t input_cnt,
                          sr_event_t event,
                          uint32_t request_id,
                          sr_val_t **output,
                          size_t *output_cnt,
                          void *private_data);

extern int
mplane_oran_reset();

extern int
mplane_oran_software_download(sr_session_ctx_t *session,
                              const char *path,
                              const sr_val_t *input,
                              const size_t input_cnt,
                              sr_event_t event,
                              uint32_t request_id,
                              sr_val_t **output,
                              size_t *output_cnt,
                              void *private_data);

extern int
o_ran_lib_sub();

extern int
o_ran_lib_init_ctx();

extern int
o_ran_lib_deinit_ctx();

extern int
mplane_oran_software_download(sr_session_ctx_t *session,
                              const char *path,
                              const sr_val_t *input,
                              const size_t input_cnt,
                              sr_event_t event,
                              uint32_t request_id,
                              sr_val_t **output,
                              size_t *output_cnt,
                              void *private_data);

extern int
module_change_subscribe();

extern int
init_o_ran_lib();

extern int
deinit_o_ran_lib();

extern int
module_change_unsubscribe();

struct oran_srv {
    sr_conn_ctx_t *sr_conn;         /**< sysrepo connection */
    sr_session_ctx_t *sr_sess;      /**< sysrepo server session */
    sr_subscription_ctx_t *sr_rpc_sub;  /**< sysrepo RPC subscription context */
    sr_subscription_ctx_t *sr_data_sub; /**< sysrepo data subscription context */
    sr_subscription_ctx_t *sr_notif_sub;    /**< sysrepo notification subscription context */
};

extern struct oran_srv oran_srv;

