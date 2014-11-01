#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "symbols.h"
#include "emitter.h"
#include "parser.h"
#include "log.h"



// ****************************************************************************
static void usage(void)
{
    fprintf(stderr,
        "Usage: light_program_assembler [-v] -o output.file source.file\n"
        "\n"
        "-v:    Verbose output. Specify multiple times for more output.\n"
        "-o:    Output file. If omitted, output is printed to stdout.\n\n");
}


// ****************************************************************************
static void banner(void)
{
    log_printf("DIY RC Light Controller light program assembler\n\n");
}


// ****************************************************************************
static char * parse_arguments(int argc, char *argv[])
{
    int verbose_count = 0;
    char *output_filename = NULL;

    opterr = 0;

    while (1) {
        int option_index = 0;
        int c;

        static struct option long_options[] = {
           {"help",    no_argument,       0,  'h' },
           {"output",  required_argument, 0,  'o' },
           {"verbose", no_argument,       0,  'v' },
           {0,         0,                 0,  0 }
        };

        c = getopt_long(argc, argv, ":ho:v", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 'h':
                usage();
                exit(0);
                break;

            case 'o':
                output_filename = optarg;
                break;

            case 'v':
                ++verbose_count;
                log_enable();
                if (verbose_count >= 2) {
                    yydebug = 1;
                }
                break;

            case ':':
                fprintf(stderr, "ERROR: Missing argument for option '%c'\n\n",
                    optopt);
                usage();
                exit(1);

            case '?':
                fprintf(stderr, "ERROR: Unknown option\n\n");
                usage();
                exit(1);

            default:
                break;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "ERROR: no input files\n");
        exit(1);
    }

    return output_filename;
}



// ****************************************************************************
int main(int argc, char *argv[])
{
    char *output_filename;
    int number_of_input_files;
    char **input_file_list;

    output_filename = parse_arguments(argc, argv);
    number_of_input_files = argc - optind;
    input_file_list = &argv[optind];

    banner();
    initialize_emitter(output_filename);
    initialize_symbols();

    yyparse();

    if (has_error_occured()) {
        return 1;
    }

    output_programs();
    return 0;
}
