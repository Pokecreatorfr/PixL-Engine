#pragma once

extern char print_buffer[1024];

/**
 * @brief Print a string to the terminal.
 * 
 * @param str The string to print. Must be null-terminated.
 */
void print_in_terminal(const char *str);
