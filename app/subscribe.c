#include "subscribe.h"
#include "mplane_api.h"

void *handle = NULL;

struct oran_srv oran_srv={0};


// This is a temp function, just to validate values
void
print_val(const sr_val_t *value)
{
    if (NULL == value) {
        return;
    }

    printf("%s ", value->xpath);

    switch (value->type) {
    case SR_CONTAINER_T:
    case SR_CONTAINER_PRESENCE_T:
        printf("(container)");
        break;
    case SR_LIST_T:
        printf("(list instance)");
        break;
    case SR_STRING_T:
        printf("= %s", value->data.string_val);
        break;
    case SR_BOOL_T:
        printf("= %s", value->data.bool_val ? "true" : "false");
        break;
    case SR_DECIMAL64_T:
        printf("= %g", value->data.decimal64_val);
        break;
    case SR_INT8_T:
        printf("= %" PRId8, value->data.int8_val);
        break;
    case SR_INT16_T:
        printf("= %" PRId16, value->data.int16_val);
        break;
    case SR_INT32_T:
        printf("= %" PRId32, value->data.int32_val);
        break;
    case SR_INT64_T:
        printf("= %" PRId64, value->data.int64_val);
        break;
    case SR_UINT8_T:
        printf("= %" PRIu8, value->data.uint8_val);
        break;
    case SR_UINT16_T:
        printf("= %" PRIu16, value->data.uint16_val);
        break;
    case SR_UINT32_T:
        printf("= %" PRIu32, value->data.uint32_val);
        break;
    case SR_UINT64_T:
        printf("= %" PRIu64, value->data.uint64_val);
        break;
    case SR_IDENTITYREF_T:
        printf("= %s", value->data.identityref_val);
        break;
    case SR_INSTANCEID_T:
        printf("= %s", value->data.instanceid_val);
        break;
    case SR_BITS_T:
        printf("= %s", value->data.bits_val);
        break;
    case SR_BINARY_T:
        printf("= %s", value->data.binary_val);
        break;
    case SR_ENUM_T:
        printf("= %s", value->data.enum_val);
        break;
    case SR_LEAF_EMPTY_T:
        printf("(empty leaf)");
        break;
    default:
        printf("(unprintable)");
        break;
    }

    switch (value->type) {
    case SR_UNKNOWN_T:
    case SR_CONTAINER_T:
    case SR_CONTAINER_PRESENCE_T:
    case SR_LIST_T:
    case SR_LEAF_EMPTY_T:
        printf("\n");
        break;
    default:
        printf("%s\n", value->dflt ? " [default]" : "");
        break;
    }
}

// This is a temp function, just to validate values
void
print_change(sr_change_oper_t op,
             sr_val_t *old_val,
             sr_val_t *new_val)
{
    switch (op) {
    case SR_OP_CREATED:
        printf("CREATED: ");
        print_val(new_val);
        break;
    case SR_OP_DELETED:
        printf("DELETED: ");
        print_val(old_val);
        break;
    case SR_OP_MODIFIED:
        printf("MODIFIED: ");
        print_val(old_val);
        printf("to ");
        print_val(new_val);
        break;
    case SR_OP_MOVED:
        printf("MOVED: %s\n", new_val->xpath);
        break;
    }
}

const char *
ev_to_str(sr_event_t ev)
{
    switch (ev) {
    case SR_EV_CHANGE:
        return "change";
    case SR_EV_DONE:
        return "done";
    case SR_EV_ABORT:
    default:
        return "abort";
    }
}

void
process_module_change(sr_change_oper_t oper,
                      sr_val_t *value)
{
    if(strstr(value->xpath, "status"))
    {
        printf("\nHit start/stop, call the API\n");
    }
    print_val(value);
}

int
module_change_cb(sr_session_ctx_t *session,
                 const char *module_name,
                 const char *xpath,
                 sr_event_t event,
                 uint32_t request_id,
                 void *private_data)
{
    sr_change_iter_t *it = NULL;
    int rc = SR_ERR_OK;
    char path[512];
    sr_change_oper_t oper;
    sr_val_t *old_value = NULL;
    sr_val_t *new_value = NULL;

    (void)request_id;
    (void)private_data;

    printf("\n\n ========== EVENT %s CHANGES: ====================================\n\n", ev_to_str(event));
    if(SR_EV_DONE == event)
    {
        printf("\n\nDone Event triggered\n\n");
        fflush(stdout);
        return SR_ERR_OK;
    }
    sprintf(path, "/%s:*//.", module_name);
    rc = sr_get_changes_iter(session, path, &it);
    if (rc != SR_ERR_OK) {
        goto cleanup;
    }

    while ((rc = sr_get_change_next(session, it, &oper, &old_value, &new_value)) == SR_ERR_OK) {
        //print_change(oper, old_value, new_value);
        process_module_change(oper, new_value);
        sr_free_val(old_value);
        sr_free_val(new_value);
    }

    printf("\n ========== END OF CHANGES =======================================");

cleanup:
    sr_free_change_iter(it);
    return SR_ERR_OK;
}

int (*load_symbol(char *symbol))()

{
    char *error = NULL;
    int (*symbol_p)();
    symbol_p = dlsym(handle, symbol);
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s\n", error);
        exit(1);
    }
    return symbol_p;
}

int
mplane_oran_software_install(sr_session_ctx_t *session,
                             const char *path,
                             const sr_val_t *input,
                             const size_t input_cnt,
                             sr_event_t event,
                             uint32_t request_id,
                             sr_val_t **output,
                             size_t *output_cnt,
                             void *private_data)
{
    int (*symbol_p)();
    int rc = SR_ERR_OK;
    ru_software_pkg_in_t in;
    ru_software_pkg_out_t *out = NULL;

    // More values can be filled and passed
    in.type = INSTALL_PKG;
    in.install_in.slot_name = input->data.string_val;

    symbol_p = load_symbol("software_install");
    rc = (*symbol_p)(&in, &out);
    if(rc != SR_ERR_OK)
        goto error;

    *output = malloc(sizeof **output);
    if(*output == NULL)
    {
        printf("malloc() failed\n");
        goto error;
    }
    *output_cnt = 1;

    (*output)[0].xpath = strdup("/o-ran-software-management:software-install/status");
    (*output)[0].type = SR_ENUM_T;
    (*output)[0].dflt = 0;
    (*output)[0].data.enum_val = strdup("STARTED");

    return SR_ERR_OK;

error:
    return -1;
}

int
subscribe_o_ran_rpcs(char *xpath,
                     int(*cb)())
{
    int rc = SR_ERR_OK;

    /* turn logging on */
    sr_log_stderr(SR_LL_WRN);

    /* Register oran specific rpc */
    O_RAN_RPC_SUBSCR(xpath, cb);

error:
    return rc;
}

int
module_change_subscribe()
{
    int rc = SR_ERR_OK;
    FILE *fp = NULL;

    printf("==== Listening for changes in all modules ====\n");

    /* turn logging on */
    sr_log_stderr(SR_LL_WRN);

    /* Open the command for reading. */
    fp = popen("sysrepoctl -l | grep '| I' | cut -d ' ' -f 1", "r");
    if (fp == NULL) {
        printf("Failed to get the list of installed modules\n" );
        rc = -1;
        goto cleanup;
    }

    char module_name[MAX_MOD_NAME_LEN] = {0};

    /* Read the output a line at a time and subscribe for module change */
    while (fgets(module_name, MAX_MOD_NAME_LEN, fp) != NULL) {

        module_name[strcspn(module_name, "\n")] = '\0';

        /* subscribe for changes in running config */
        rc = sr_module_change_subscribe(oran_srv.sr_sess, module_name, NULL,
                                  module_change_cb, NULL, 0, 0, &oran_srv.sr_data_sub);
        if (rc != SR_ERR_OK) {
            printf("Failed to subscribe module(%s) for change", module_name);
            goto cleanup;
        }
    }

    /* close */
    pclose(fp);

cleanup:
    return rc;
}

int init_sysrepo()
{
    int rc = SR_ERR_OK;

    /* connect to sysrepo */
    rc = sr_connect(SR_CONN_CACHE_RUNNING, &oran_srv.sr_conn);
    if (rc != SR_ERR_OK) {
        printf("Faild to connect to sysrepo\n");
        goto error;
    }

    /* start session */
    rc = sr_session_start(oran_srv.sr_conn, SR_DS_RUNNING, &oran_srv.sr_sess);
    if (rc != SR_ERR_OK) {
        printf("Faild to create sysrepo session\n");
        goto error;
    }
error:
    return rc;
}

char *read_oper_data_file()
{
    FILE * fptr;
    int read_len = 0;
    int buff_len = 0;
    char *buff = NULL;
    char buffer[MAX_BUFF_SIZE+1]={0};

    buff = malloc(1);

    fptr = fopen("/tmp/oper.xml", "r");
    if(fptr == NULL)
    {
        printf("Failed to open file\n");
        return 0;
    }

    while(1)
    {
        read_len = fread(buffer, sizeof(char), MAX_BUFF_SIZE, fptr);
        if(read_len > 0)
        {
            buff = realloc(buff, buff_len + read_len);
            strcpy(buff + buff_len, buffer);
            buff_len += read_len;
        }
        if(read_len < MAX_BUFF_SIZE)
        {
            if(feof(fptr))
            {
                 buff[buff_len]='\0';
                 break;
            }
            else
            {
                printf("Error occured");
                free(buff);
                buff = NULL;
                break;
            }
        }
    }

    fclose(fptr);
    return buff;
}

int sw_inventory_get_items_cb(sr_session_ctx_t *session,
                              const char *module_name,
                              const char *xpath,
                              const char *request_xpath,
                              uint32_t request_id,
                              struct lyd_node **parent,
                              void *private_data)
{
    char *buff = NULL;

    /* Read the xml */
    buff = read_oper_data_file();

    /* Populate operational data */
    *parent = lyd_parse_mem((struct ly_ctx *)sr_get_context(oran_srv.sr_conn),
                    buff,
                    LYD_XML,
                    LYD_OPT_TRUSTED |  LYD_OPT_GET);

#if 0
    /* Such code will be required if we need to create slot from content over filesystem */

    *parent = lyd_new_path(NULL,
                    sr_get_context(sr_session_get_connection(oran_srv.sr_sess)),
                    "/o-ran-software-management:software-inventory/software-slot/name",
                    "SOFTWARE_SLOT1_RUNNING",
                    0,
                    0);
    lyd_new_path(*parent,
                    NULL,
                    "/o-ran-software-management:software-inventory/software-slot/status",
                    "VALID",
                    0,
                    0);
#endif
    return 0;
}

int subscribe_operation_data()
{
    int rc = SR_ERR_OK;
    rc = sr_oper_get_items_subscribe(oran_srv.sr_sess,
                                     "o-ran-software-management",
                                     "/o-ran-software-management:software-inventory",
                                     sw_inventory_get_items_cb,
                                     NULL,
                                     SR_SUBSCR_CTX_REUSE,
                                     &oran_srv.sr_rpc_sub);
    if (rc != SR_ERR_OK) {
        return -1;
    }

    return rc;
}

#if 0
int subscribe_customer_rpc()
{
    printf("Need to add by customer");
    subscribe_o_ran_rpcs("/o-ran-software-management:software-install", mplane_oran_software_install);
    return 0;
}
#endif

int o_ran_lib_init()
{
    int rc = SR_ERR_OK;

    rc = init_sysrepo();
    if(rc != SR_ERR_OK)
    {
        goto error;
    }

    rc = module_change_subscribe();
    if(rc != SR_ERR_OK)
    {
        goto error;
    }
#if 0
    rc = subscribe_customer_rpc();
    if(rc != SR_ERR_OK)
    {
        goto error;
    }
#endif
    rc = subscribe_operation_data();
    if(rc != SR_ERR_OK)
    {
        goto error;
    }

error:
    return rc ? EXIT_FAILURE : EXIT_SUCCESS;
}

int o_ran_lib_deinit()
{
    sr_unsubscribe(oran_srv.sr_rpc_sub);
    sr_unsubscribe(oran_srv.sr_data_sub);

    /*
     * sr_unsubscribe(oran_srv.sr_notif_sub);
     */

    sr_disconnect(oran_srv.sr_conn);
    return SR_ERR_OK;
}

int
mplane_oran_software_activate(sr_session_ctx_t *session,
                              const char *path,
                              const sr_val_t *input,
                              const size_t input_cnt,
                              sr_event_t event,
                              uint32_t request_id,
                              sr_val_t **output,
                              size_t *output_cnt,
                              void *private_data)
{
    int (*symbol_p)();
    int rc = SR_ERR_OK;
    ru_software_pkg_in_t in;
    ru_software_pkg_out_t *out = NULL;

    in.type = ACTIVATE_PKG;
    in.activate_in.slot_name = input->data.string_val;

    symbol_p = load_symbol("software_activate");
    rc = (*symbol_p)(&in, &out);
    if(rc != SR_ERR_OK)
        goto error;

    *output = malloc((sizeof **output)*2);
    if(*output == NULL)
    {
        printf("malloc() failed\n");
        goto error;
    }
    *output_cnt = 2;

    (*output)[0].xpath = strdup("/o-ran-software-management:software-activate/status");
    (*output)[0].type = SR_ENUM_T;
    (*output)[0].dflt = 0;
    (*output)[0].data.enum_val = strdup("STARTED");

    (*output)[1].xpath = strdup("/o-ran-software-management:software-activate/notification-timeout");
    (*output)[1].type = SR_INT32_T;
    (*output)[1].dflt = 0;
    (*output)[1].data.int32_val = 10;

    return SR_ERR_OK;

error:
    return -1;
}

int
mplane_oran_file_upload(sr_session_ctx_t *session,
                        const char *path,
                        const sr_val_t *input,
                        const size_t input_cnt,
                        sr_event_t event,
                        uint32_t request_id,
                        sr_val_t **output,
                        size_t *output_cnt,
                        void *private_data)
{
    int (*symbol_p)();
    int rc = SR_ERR_OK;
    ru_file_mgmt_in_t in;
    ru_file_mgmt_out_t *out = NULL;

    in.type = FILE_MGMT_UPLOAD;
    in.file_upload_in.local_logical_file_path = input[0].data.string_val;
    in.file_upload_in.remote_file_path = input[1].data.string_val;

    symbol_p = load_symbol("file_upload");
    rc = (*symbol_p)(&in, &out);
    if(rc != SR_ERR_OK)
        goto error;

    *output = malloc((sizeof **output)*2);
    if(*output == NULL)
    {
        printf("malloc() failed\n");
        goto error;
    }
    *output_cnt = 2;

    (*output)[0].xpath = strdup("/o-ran-file-management:file-upload/status");
    (*output)[0].type = SR_ENUM_T;
    (*output)[0].dflt = 0;
    (*output)[0].data.enum_val = strdup("FAILURE");

    (*output)[1].xpath = strdup("/o-ran-file-management:file-upload/reject-reason");
    (*output)[1].type = SR_STRING_T;
    (*output)[1].dflt = 0;
    (*output)[1].data.string_val = strdup("Some failure to feed");

    return SR_ERR_OK;

error:
    return -1;

}

int
mplane_oran_retrieve_file_list(sr_session_ctx_t *session,
                               const char *path,
                               const sr_val_t *input,
                               const size_t input_cnt,
                               sr_event_t event,
                               uint32_t request_id,
                               sr_val_t **output,
                               size_t *output_cnt,
                               void *private_data)
{
    int (*symbol_p)();
    int rc = SR_ERR_OK;
    ru_file_mgmt_in_t in;
    ru_file_mgmt_out_t *out = NULL;

    in.type = FILE_MGMT_UPLOAD;
    in.file_retrieve_in.logical_path = input[0].data.string_val;
    in.file_retrieve_in.file_name_filter = input[1].data.string_val;

    symbol_p = load_symbol("retrieve_file_list");
    rc = (*symbol_p)(&in, &out);
    if(rc != SR_ERR_OK)
        goto error;

    *output = malloc((sizeof **output)*2);
    if(*output == NULL)
    {
        printf("malloc() failed\n");
        goto error;
    }
    *output_cnt = 2;

    (*output)[0].xpath = strdup("/o-ran-file-management:retrieve-file-list/status");
    (*output)[0].type = SR_ENUM_T;
    (*output)[0].dflt = 0;
    (*output)[0].data.enum_val = strdup("FAILURE");

    (*output)[1].xpath = strdup("/o-ran-file-management:retrieve-file-list/reject-reason");
    (*output)[1].type = SR_STRING_T;
    (*output)[1].dflt = 0;
    (*output)[1].data.string_val = strdup("Some failure to feed");

    return SR_ERR_OK;

error:
    return -1;

}

int
mplane_oran_file_download(sr_session_ctx_t *session,
                          const char *path,
                          const sr_val_t *input,
                          const size_t input_cnt,
                          sr_event_t event,
                          uint32_t request_id,
                          sr_val_t **output,
                          size_t *output_cnt,
                          void *private_data)
{
    int (*symbol_p)();
    int rc = SR_ERR_OK;
    ru_file_mgmt_in_t in;
    ru_file_mgmt_out_t *out = NULL;

    in.type = FILE_MGMT_UPLOAD;
    in.file_download_in.local_logical_file_path = input[0].data.string_val;
    in.file_download_in.remote_file_path = input[1].data.string_val;

    symbol_p = load_symbol("file_download");
    rc = (*symbol_p)(&in, &out);
    if(rc != SR_ERR_OK)
        goto error;

    *output = malloc((sizeof **output)*2);
    if(*output == NULL)
    {
        printf("malloc() failed\n");
        goto error;
    }
    *output_cnt = 2;

    (*output)[0].xpath = strdup("/o-ran-file-management:file-download/status");
    (*output)[0].type = SR_ENUM_T;
    (*output)[0].dflt = 0;
    (*output)[0].data.enum_val = strdup("FAILURE");

    (*output)[1].xpath = strdup("/o-ran-file-management:file-download/reject-reason");
    (*output)[1].type = SR_STRING_T;
    (*output)[1].dflt = 0;
    (*output)[1].data.string_val = strdup("Some failure to feed");

    return SR_ERR_OK;

error:
    return -1;

}

int mplane_oran_reset()
{
    printf("Do reset related stuff here\n");
    return 0;
}

int
mplane_oran_software_download(sr_session_ctx_t *session,
                              const char *path,
                              const sr_val_t *input,
                              const size_t input_cnt,
                              sr_event_t event,
                              uint32_t request_id,
                              sr_val_t **output,
                              size_t *output_cnt,
                              void *private_data)
{
    int (*symbol_p)();
    int rc = SR_ERR_OK;
    ru_software_pkg_in_t in;
    ru_software_pkg_out_t *out = NULL;

    in.type = DOWNLOAD_PKG;
    in.downlod_in.uri = input->data.string_val;

    symbol_p = load_symbol("software_download");
    rc = (*symbol_p)(&in, &out);
    if(rc != SR_ERR_OK)
        goto error;

    *output = malloc((sizeof **output)*2);
    if(*output == NULL)
    {
        printf("malloc() failed\n");
        goto error;
    }
    *output_cnt = 2;

    (*output)[0].xpath = strdup("/o-ran-software-management:software-download/status");
    (*output)[0].type = SR_ENUM_T;
    (*output)[0].dflt = 0;
    (*output)[0].data.enum_val = strdup("STARTED");

    (*output)[1].xpath = strdup("/o-ran-software-management:software-download/notification-timeout");
    (*output)[1].type = SR_INT32_T;
    (*output)[1].dflt = 0;
    (*output)[1].data.int32_val = 10;

    return SR_ERR_OK;

error:
    return -1;
}


int
o_ran_lib_sub()
{
#define MAX_SIZE_XPATH 256
    int i = 0;
    //int (*subscribe_o_ran_rpcs_fptr)(char *xpath, int(*cb)());
    //char *error;
    char rpcs[][MAX_SIZE_XPATH] = {
        "/o-ran-software-management:software-download",
        "/o-ran-software-management:software-install",
        "/o-ran-software-management:software-activate",
        "/o-ran-operations:reset",
        "/o-ran-file-management:file-upload",
        "/o-ran-file-management:retrieve-file-list",
        "/o-ran-file-management:file-download"
    };
    int (*cbs[])() = {
            mplane_oran_software_download,
            mplane_oran_software_install,
            mplane_oran_software_activate,
            mplane_oran_reset,
            mplane_oran_file_upload,
            mplane_oran_retrieve_file_list,
            mplane_oran_file_download,
            NULL
    };

    do {
        printf("subscribing callback(%d): %s\n", i, rpcs[i]);
        subscribe_o_ran_rpcs(rpcs[i], cbs[i]);
    } while(cbs[++i]);
    return 0;
}


int
o_ran_lib_init_ctx()
{
    int rc = SR_ERR_OK;
    handle = dlopen ("libruapp.so", RTLD_LAZY);
    if (!handle) {
        fprintf (stderr, "%s\n", dlerror());
        exit(1);
    }

    dlerror();    /* Clear any existing error */
    rc = o_ran_lib_init();
    rc = o_ran_lib_sub();
    return rc;
}

int
o_ran_lib_deinit_ctx()
{
    o_ran_lib_deinit();
    dlclose(handle);
    return 0;
}
