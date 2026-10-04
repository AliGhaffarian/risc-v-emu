#include "cpu.h"
#include "helper.h"
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
};

struct args_struct args = {
    .program_file = "",
};

char *usage_help = "usage: rv_emu -p[rogram-file] PROGRAM_FILENAME";

struct option long_options[] = {
    {.name    = "program-file",
     .has_arg = required_argument,
     .flag    = NULL,
     .val     = 'c'},
};

void print_help_and_quit()
{
    puts(usage_help);
    exit(1);
}

void handle_args(int argc, char **argv)
{
    int required_args = 1;
    int option_index  = -1;
    int err;
    int optchar;
    while(1) {
        optchar = getopt_long(argc, argv, "hp:", long_options, &option_index);
        if(optchar == -1) {
            break;
        }
        switch(optchar) {
        case 'p':
            args.program_file = strdup(optarg);
            required_args--;
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

    handle_args(argc, argv);

    program_file = fopen(args.program_file, "r");
    if(!program_file) {
        puts(strerror(errno));
        exit(1);
    }

    init_rv64_cpu(&cpu, NULL);
    free_memory_in_machine = cpu.opt.mem_size;

    while(current_read_bytes && free_memory_in_machine) {
        current_read_bytes = fread(cpy_buff, BUFSIZ, 1, program_file);
        total_read_bytes += current_read_bytes;

        bytes_to_write = current_read_bytes > free_memory_in_machine
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
