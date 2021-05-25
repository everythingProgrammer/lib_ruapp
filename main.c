#include <signal.h>
#include <unistd.h>
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
    /* loop until ctrl-c is pressed / SIGINT is received */
    rc = module_change_subscribe();
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN);
    while (!exit_application) {
        sleep(100);
    }

    /* Unsubscribe the watch for module change */
    module_change_unsubscribe();
    printf("Application exit requested, exiting.\n");
    return rc;
}

