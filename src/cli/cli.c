#include "cpu.h"
#include "helper.h"
#include "logger.h"
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Stores parsed command line arguments.
 */
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

char *usage_help =
    "usage: rv_emu -l[og-level] LOG_LEVEL -p[rogram-file] PROGRAM_FILENAME";

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
    FILE *program_file            = NULL;
    struct rv64_cpu cpu           = {0};
    char cpy_buff[BUFSIZ]         = {0};
    uint64_t program_file_size    = 0;
    size_t total_read_bytes       = 0;
    size_t current_read_bytes     = -1;
    size_t free_memory_in_machine = 0;
    size_t bytes_to_write         = 0;
    int err                       = 0;
    struct cpu_opt c_opt          = {
        .mem_size = BASE_MEM_SIZE,
        .regs     = BASE_REGS_NUM,
    };

    handle_args(argc, argv);
    c_opt.mem_size = args.mem_size,

    program_file = fopen(args.program_file, "r");
    if(!program_file) {
        puts(strerror(errno));
        exit(1);
    }

    err = init_rv64_cpu(&cpu, NULL);
    if(err) {
        logger(LOG_ERROR, stdout, "error initing the cpu\n");
        exit(1);
    }
    logger(LOG_DEBUG, stdout, "init cpu success\n");

    free_memory_in_machine = cpu.opt.mem_size;

    while(current_read_bytes && free_memory_in_machine) {
        current_read_bytes = fread(cpy_buff, 1, BUFSIZ, program_file);
        total_read_bytes += current_read_bytes;

        bytes_to_write = current_read_bytes < free_memory_in_machine
                             ? current_read_bytes
                             : free_memory_in_machine;
        memcpy(cpu.mem + total_read_bytes, cpy_buff, bytes_to_write);

        free_memory_in_machine -= bytes_to_write;
    }

    if(current_read_bytes) {
        puts("not enough memory in machine");
        exit(1);
    }

    mainloop_rv64_cpu(&cpu);
    debug_dump_cpu(&cpu);
}
