#include "ft_ping.h"

options     *parse_argv(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stderr, "%s: usage error: destination addresse or ip required\n", argv[0]);
        exit(EXIT_FAILURE);
        return NULL;
    }

    return NULL;
}
