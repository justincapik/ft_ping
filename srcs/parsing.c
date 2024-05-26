#include "ft_ping.h"

int    parse_argv(int argc, char **argv, options *opts)
{
    if (argc == 1)
    {
        fprintf(stderr, "%s: usage error: destination addresse or ip required\n", argv[0]);
        return FALSE;
    }

    opts->ttl = 64;
    opts->flags = 0;
    opts->host = argv[argc - 1];
    opts->count = -1;
    if (opts->count <= 0)
        opts->flags |= OPTS_COUNT;

    if (!(opts->flags & OPTS_COUNT) && opts->count <= 0)
    {
        fprintf(stderr, "%s: invalid argument: '0': out of range: 1 <= value <= 9223372036854775807\n", argv[0]);
        return FALSE;
    }
    return TRUE;
}
