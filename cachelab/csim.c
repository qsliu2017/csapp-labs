#include "cachelab.h"
#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define LOG(...) (verbose ? printf(__VA_ARGS__) : (void)0)
#define ERROR(...) fprintf(stderr, __VA_ARGS__)

int hit_count = 0, miss_count = 0, eviction_count = 0,
    s, E, b;
bool verbose = false;
void extract_trace(char *const trace, char *op, long *addr, int *size)
{
    sscanf(trace, " %1c %lx,%d", op, addr, size);
}
typedef struct
{
    bool valid;
    int lru;
    long tag;
} cache_line_t;

long get_set_index(long addr)
{
    return (addr >> b) & ((1 << s) - 1);
}
long get_tag(long addr)
{
    return (addr >> (s + b)) & ((1 << (64 - (s + b))) - 1);
}
void sim(cache_line_t **cache_sets, long addr)
{
    long set_index = get_set_index(addr), tag = get_tag(addr);
    cache_line_t *cache_set = cache_sets[set_index];
    bool hit = false;
    int eviction, max_lru = -1;
    for (int i = 0; i < E; i++)
    {
        if (cache_set[i].valid)
        {
            if (cache_set[i].tag == tag)
            {
                hit = true;
                cache_set[i].lru = 0;
            }
            else if (++(cache_set[i].lru) > max_lru)
            {
                eviction = i;
                max_lru = cache_set[i].lru;
            }
        }
        else
        {
            eviction = i;
            max_lru = __INT_MAX__;
        }
    }
    if (hit)
    {
        LOG(" hit");
        hit_count++;
    }
    else
    {
        LOG(" miss");
        miss_count++;
        if (cache_set[eviction].valid)
        {
            LOG(" eviction");
            eviction_count++;
        }
        cache_set[eviction].valid = true;
        cache_set[eviction].tag = tag;
        cache_set[eviction].lru = 0;
    }
}

int main(int argc, char *const *argv)
{
    char const *tracefile;
    for (int opt; - 1 != (opt = getopt(argc, argv, "hvs:E:b:t:"));)
    {
        switch (opt)
        {
        case 'v':
            verbose = true;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            tracefile = (char *)optarg;
            break;
        case 'h':
        default:
            printf("Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", argv[0]);
            return 1;
        }
    }

    FILE *tfile = fopen(tracefile, "r");
    if (!tfile)
    {
        ERROR("Unable to open tracefile: %s\n", tracefile);
        return 2;
    }

    cache_line_t **cache_sets = (cache_line_t **)calloc(1 << s, sizeof(cache_line_t *));
    for (long i = 0, set_num = 1 << s; i < set_num; i++)
    {
        cache_sets[i] = (cache_line_t *)calloc(E, sizeof(cache_line_t));
    }

    char op;
    long addr;
    int size;
    for (char buffer[256]; NULL != fgets(buffer, 256, tfile);)
    {
        extract_trace(buffer, &op, &addr, &size);
        switch (op)
        {
        case 'L':
        case 'S':
            LOG("%c %lx,%d", op, addr, size);
            sim(cache_sets, addr);
            LOG("\n");
            break;
        case 'M':
            LOG("%c %lx,%d", op, addr, size);
            sim(cache_sets, addr);
            LOG(" hit");
            hit_count++;
            LOG("\n");
            break;
        }
    }
    printSummary(hit_count, miss_count, eviction_count);

    { /*Clean up*/
        fclose(tfile);
        for (long i = 0, set_num = 1 << s; i < set_num; i++)
        {
            free(cache_sets[i]);
        }
        free(cache_sets);
    }
    return 0;
}
