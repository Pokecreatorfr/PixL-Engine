#include "engine/command_parser.h"

#include <string.h>

#include "engine/print.h"

bool no_display_used = false;
bool no_input_used = false;
bool no_audio_used = false;
bool help_used = false;

void command_parser_parse(int argc, char *argv[])
{
    int i;

    no_display_used = false;
    no_input_used = false;
    no_audio_used = false;
    help_used = false;

    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--no-display") == 0) {
            no_display_used = true;
        } else if (strcmp(argv[i], "--no-input") == 0) {
            no_input_used = true;
        } else if (strcmp(argv[i], "--no-audio") == 0) {
            no_audio_used = true;
        } else if (strcmp(argv[i], "--help") == 0) {
            help_used = true;
        }
    }
}

void command_parser_print_help(const char *program_name)
{
    print_in_terminal("Usage: ");
    if (program_name != 0 && program_name[0] != '\0') {
        print_in_terminal(program_name);
    } else {
        print_in_terminal("PixL-Engine");
    }
    print_in_terminal(" [options]\n");
    print_in_terminal("Options:\n");
    print_in_terminal("  --no-display   Disable display output\n");
    print_in_terminal("  --no-input     Disable input handling\n");
    print_in_terminal("  --no-audio     Disable audio output\n");
    print_in_terminal("  --help         Show this help message\n");
}
