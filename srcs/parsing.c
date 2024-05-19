#include "ft_ping.h"

options     *parse_argv(int argc, char **argv)
{
    options *opts;

    if (argc == 1)
    {
        fprintf(stderr, "%s: usage error: destination addresse or ip required\n", argv[0]);
        exit(EXIT_FAILURE);
        return NULL;
    }

    opts = (options*)malloc(sizeof(options));

    opts->count = -1;
    opts->ttl = 64;
    opts->verbose = FALSE;
    opts->host = argv[argc - 1];

    return opts;
}
