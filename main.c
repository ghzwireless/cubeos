
#include <stdio.h>

#include "shell.h"

#ifdef MODULE_GPS
    extern int location_demo(int argc, char **argv);
#endif

#ifdef MODULE_HAM
#include "ham_shell.h"
#endif

int hello_world(int argc, char **argv) {
    /* Suppress compiler errors */
    (void)argc;
    (void)argv;
    printf("hello world!\n");
    return 0;
}

const shell_command_t shell_commands[] = {
    {"hello", "prints hello world", hello_world},
#ifdef MODULE_GPS
    {"gps", "Gets GPS Data", location_demo},
#endif
#ifdef MODULE_HAM
    HAM_SHELL_COMMANDS
#endif
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Welcome to KubOS! Initializing...");

#ifdef MODULE_HAM
    ham_cmd_init();
#endif

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
