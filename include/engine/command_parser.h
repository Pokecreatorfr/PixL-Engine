#pragma once

#include <stdbool.h>

extern bool no_display_used;
extern bool no_input_used;
extern bool no_audio_used;
extern bool help_used;

/**
 * @brief Parse command-line arguments.
 * @param argc The number of command-line arguments.
 * @param argv An array of pointers to the command-line arguments.
 */
void command_parser_parse(int argc, char *argv[]);

/**
 * @brief Print the help message.
 * @param program_name The name of the program.
 */
void command_parser_print_help(const char *program_name);
