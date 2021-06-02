/*
 * This is a driver program for the library and other static codes
 */

#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#include "subscribe.h"

volatile int exit_application = 0;
static void
sigint_handler(int signum)
{
    (void)signum;

    exit_application = 1;
}

int main(){
    int rc;

    /* Initialize the oran library */
    rc = o_ran_lib_init_ctx();

    /* loop until ctrl-c is pressed / SIGINT is received */
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN);
    while (!exit_application) {
        sleep(100);
    }

    /* Unsubscribe the watch for module change */
    o_ran_lib_deinit_ctx();
    printf("Application exit requested, exiting.\n");
    return rc;
}
