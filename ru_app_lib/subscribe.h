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
#ifndef __RU_APP_LIB__
#define __RU_APP_LIB__

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sysrepo.h"

extern void print_val(const sr_val_t *value);

extern void print_change(sr_change_oper_t op, sr_val_t *old_val, sr_val_t *new_val);

extern const char * ev_to_str(sr_event_t ev);

extern int module_change_cb(sr_session_ctx_t *session, const char *module_name,
                            const char *xpath, sr_event_t event,
                            uint32_t request_id, void *private_data);

extern int module_change_subscribe();

extern int module_change_unsubscribe();

#endif
