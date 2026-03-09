#include "engine/print.h"

char print_buffer[1024];

static unsigned long print_strlen(const char *str)
{
    unsigned long len = 0;
    
    if (str == 0) {
        return 0;
    }

    while (str[len] != '\0') {
        ++len;
    }

    return len;
}

#ifdef _WIN32

#define PRINT_STD_OUTPUT_HANDLE ((unsigned long)-11)
#define PRINT_STDCALL __attribute__((stdcall))
#define PRINT_DLLIMPORT __attribute__((dllimport))

typedef void *print_handle_t;
typedef int print_bool_t;

PRINT_DLLIMPORT print_handle_t PRINT_STDCALL GetStdHandle(unsigned long standard_handle);
PRINT_DLLIMPORT print_bool_t PRINT_STDCALL WriteFile(
    print_handle_t file_handle,
    const void *buffer,
    unsigned long bytes_to_write,
    unsigned long *bytes_written,
    void *overlapped);

void print_in_terminal(const char *str)
{
    unsigned long len = print_strlen(str);
    unsigned long written = 0;

    if (len == 0) {
        return;
    }

    (void)WriteFile(GetStdHandle(PRINT_STD_OUTPUT_HANDLE), str, len, &written, 0);
}

#else

extern long write(int fd, const void *buffer, unsigned long count);

void print_in_terminal(const char *str)
{
    unsigned long len = print_strlen(str);

    if (len == 0) {
        return;
    }

    (void)write(1, str, len);
}

#endif
