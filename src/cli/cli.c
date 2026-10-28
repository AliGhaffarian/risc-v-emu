#include "cpu.h"
#include "helper.h"
#include "io_dev.h"
#include "io_dev_memory.h"
#include "logger.h"
#include "machine.h"
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Stores parsed command line arguments.
 */

#define BASE_MEM_SIZE 1024
struct args_struct {
    char *program_file;
    uint64_t mem_size;
    int log_level;
};

struct args_struct args = {
    .program_file = "",
    .mem_size     = BASE_MEM_SIZE,
    .log_level    = LOG_INFO,
};

char *usage_help = "usage: rv_emu -l[og-level] LOG_LEVEL -m[mem-size] "
                   "BASE10_MEMSIZE -p[rogram-file] PROGRAM_FILENAME";

struct option long_options[] = {
    {.name    = "program-file",
     .has_arg = required_argument,
     .flag    = NULL,
     .val     = 'p'},
    {.name    = "mem-size",
     .has_arg = optional_argument,
     .flag    = NULL,
     .val     = 'm'},
    {.name    = "log-level",
     .has_arg = required_argument,
     .flag    = NULL,
     .val     = 'l'},
};

void print_help_and_quit()
{
    puts(usage_help);

    printf("log levels:\n");
    for(int i = 1; i < LOG_DEBUG + 1; i++) {
        printf("%s, ", LOG_LEVELS2STR[i]);
    }
    puts("");

    exit(1);
}

void handle_args(int argc, char **argv)
{
    int required_args = 1;
    int option_index  = -1;
    int err;
    int optchar;
    while(1) {
        optchar =
            getopt_long(argc, argv, "hp:l:m:", long_options, &option_index);
        if(optchar == -1) {
            break;
        }
        switch(optchar) {
        case 'p':
            args.program_file = strdup(optarg);
            required_args--;
            break;
        case 'm':
            // TODO: parse hex and binary formats

            // NOLINTNEXTLINE
            args.mem_size = strtoul(optarg, NULL, 10);
            if(errno) {
                printf("invalid mem size: %s", optarg);
                print_help_and_quit();
            }
        case 'l':
            args.log_level = enum_from_string_log_levels(optarg);
            if(!args.log_level) {
                printf("invalid log level: %s\n", optarg);
                print_help_and_quit();
            }
            current_log_level = args.log_level;
            break;
        case 'h':
            print_help_and_quit();
            break;
        default:
            break;
        }
    }
    if(required_args) {
        printf("unmet required args: %d\n", required_args);
        print_help_and_quit();
    }
}

int main(int argc, char **argv)
{
    FILE *program_file                          = NULL;
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu = {0};
    int err                                     = 0;
    struct cpu_opt c_opt                        = {
        .regs = BASE_REGS_NUM,
    };
    _cleanup_freep_sysbus_ struct sysbus *sbus = calloc(1, sizeof(*sbus));
    _cleanup_free_io_dev_meta_data_ struct io_dev_metadata *mdata = NULL;
    _cleanup_freep_io_dev_memory_ struct io_dev_memory *mem       = NULL;
    _cleanup_free_ char *input_buf                                = NULL;

    handle_args(argc, argv);

    program_file = fopen(args.program_file, "r");
    if(!program_file) {
        puts(strerror(errno));
        exit(1);
    }

    input_buf = calloc(1, args.mem_size);
    if(!input_buf) {
        puts(strerror(errno));
        return errno;
    }

    (void)fread(input_buf, args.mem_size, 1, program_file);
    err = new_metadata_with_mem(&mdata, (uint8_t *)input_buf, args.mem_size);
    if(err) {
        logger(
            LOG_ERROR,
            stdout,
            "failed to allocate a new meta data for memory device");
        return err;
    }
    freep((void *)&input_buf);

    mem = new_io_dev_memory(&mdata);
    if(!mem) {
        logger(LOG_ERROR, stdout, "failed to allocate a new memory device");
        return err;
    }

    err = sysbus_memory_map_iodev(sbus, (struct io_dev **)&mem);
    if(err) {
        logger(LOG_ERROR, stdout, "failed to memory map memory device");
        return err;
    }

    err = init_rv64_cpu(&cpu, sbus, NULL);
    if(err) {
        logger(LOG_ERROR, stdout, "error initing the cpu\n");
        exit(1);
    }
    logger(LOG_DEBUG, stdout, "init cpu success\n");

    if(ferror(program_file)) {
        puts(strerror(errno));
        exit(1);
    }

    if(!feof(program_file)) {
        long end_of_file = 0;
        (void)fseek(program_file, 0, SEEK_END);
        end_of_file = ftell(program_file);
        printf("not enough memory in machine, need %lu bytes", end_of_file);
        debug_dump_cpu(&cpu);
        exit(1);
    }

    (void)fclose(program_file);

    machine_mainloop(&cpu, sbus);
    debug_dump_cpu(&cpu);
}
