/**
 * @file application_changes_example.c
 * @author Michal Vasko <mvasko@cesnet.cz>
 * @brief example of an application handling changes
 *
 * @copyright
 * Copyright (c) 2019 CESNET, z.s.p.o.
 *
 * This source code is licensed under BSD 3-Clause License (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://opensource.org/licenses/BSD-3-Clause
 */
//#define _QNX_SOURCE /* sleep */
//#define _GNU_SOURCE

#include "subscribe.h"

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

void
print_change(sr_change_oper_t op, sr_val_t *old_val, sr_val_t *new_val)
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
process_module_change(sr_change_oper_t oper, sr_val_t *value)
{
    if(strstr(value->xpath, "status"))
    {
        printf("\nHit start/stop, call the API\n");
    }
    print_val(value);
}

int
module_change_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath, sr_event_t event,
        uint32_t request_id, void *private_data)
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

sr_conn_ctx_t *connection = NULL;

int
module_change_subscribe()
{
    sr_session_ctx_t *session = NULL;
    sr_subscription_ctx_t *subscription = NULL;
    int rc = SR_ERR_OK;
    FILE *fp = NULL;

    printf("Watching for changes in all modules\n");

    /* turn logging on */
    sr_log_stderr(SR_LL_WRN);

    /* connect to sysrepo */
    rc = sr_connect(0, &connection);
    if (rc != SR_ERR_OK) {
        goto cleanup;
    }

    /* start session */
    rc = sr_session_start(connection, SR_DS_RUNNING, &session);
    if (rc != SR_ERR_OK) {
        goto cleanup;
    }

    /* Open the command for reading. */
    fp = popen("sysrepoctl -l | grep '| I' | cut -d ' ' -f 1", "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    #define MAX_MOD_NAME_LEN 128
    char module_name[MAX_MOD_NAME_LEN] = {0};

    /* Read the output a line at a time - output it. */
    while (fgets(module_name, MAX_MOD_NAME_LEN, fp) != NULL) {
        module_name[strcspn(module_name, "\n")] = '\0';
        /* subscribe for changes in running config */
        rc = sr_module_change_subscribe(session, module_name, NULL,
                                  module_change_cb, NULL, 0, 0, &subscription);
        if (rc != SR_ERR_OK) {
            goto cleanup;
        }
    }

    /* close */
    pclose(fp);

    printf("\n\n ========== LISTENING FOR CHANGES ==========\n\n");

cleanup:
    return rc ? EXIT_FAILURE : EXIT_SUCCESS;
}

int
module_change_unsubscribe()
{
    sr_disconnect(connection);
    return SR_ERR_OK;
}
