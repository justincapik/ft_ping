#include "ft_ping.h"

options     *parse_argv(int argc, char **argv)
{
    options *opts;

    if (argc == 1)
    {
        fprintf(stderr, "%s: usage error: destination addresse or ip required\n", argv[0]);
        return NULL;
    }

    opts = (options*)malloc(sizeof(options));

    opts->count = -1;
    opts->ttl = 64;
    opts->flags = 0;
    opts->host = argv[argc - 1];

    if (opts->flags & OPTS_COUNT > 0 && opts->count <= 0)
    {
        fprintf(stderr, "%s: invalid argument: '0': out of range: 1 <= value <= 9223372036854775807\n", argv[0]);
        return NULL;
    }

    return opts;
}
