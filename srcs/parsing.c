#include "ft_ping.h"

int     isnumber(char *str)
{
    size_t i;
    size_t len;

    len = strlen(str);
    i = 0;

    if (len == 0)
        return FALSE;
    if (str[0] == '-' || str[0] == '+')
        ++i;
    while (i < len)
    {
        if (str[i] < '0' || str[i] > '9')
            return FALSE;
        ++i;
    }
    return TRUE;
}

int     parse_argv(int argc, char **argv, options *opts)
{
    char usage[] = "Usage\n\tping [options] <destination>\n\nOptions:\
    \t<destination>";
    if (argc == 1)
    {
        fprintf(stderr, "%s: usage error: destination addresse or ip required\n", argv[0]);
        return FALSE;
    }

    opts->ttl = 64;
    opts->flags = 0;
    opts->host = argv[argc - 1];
    opts->count = -1;

    for (int i = 1; i < argc - 1; ++i)
    {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
            opts->flags |= OPTS_VERBOSE;
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printf("%s", usage);
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(argv[i], "-c") == 0 && isnumber(argv[i+1]) && i < argc - 2)
        {
            opts->count = atol(argv[i+1]);
            if (opts->count <= 0 || opts->count > LONG_MAX)
            {
                fprintf(stderr, "%s: invalid argument: '%s': out of range: 1 <= value <= 9223372036854775807\n",
                    argv[0], argv[i+1]);
                return FALSE;
            }
            ++i;
        }
        else if (strcmp(argv[i], "-t") == 0 && isnumber(argv[i+1]) && i < argc - 2)
        {
            long tmp = atol(argv[i+1]);
            if (tmp < 0 || tmp > 255)
            {
                fprintf(stderr, "%s: invalid argument: '%s': out of range: 0 <= value <= 255\n",
                    argv[0], argv[i+1]);
                return FALSE;
            }
            opts->ttl = tmp;
            ++i;
        }
        else
        {
            fprintf(stderr, "%s: invalid argument: %s\n", argv[0], argv[i]);
            return FALSE;
        }
    }

    return TRUE;
}
