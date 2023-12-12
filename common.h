#if !defined(_COMMON_H)
#define _COMMON_H
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>

#if !defined(ARRAYSIZE)
#define ARRAYSIZE(_a) sizeof(_a)/sizeof(_a[0])
#endif

typedef int (*CommandHandler_f)(void* out, const void* in);
typedef int (*CommandParser_f)(void* out, const char* in);

typedef struct {
    const char* name;
    CommandHandler_f handler;
    void* out;
    const void* in;
    CommandParser_f parser;
    void* parseOut;
    const char* parseIn;
} CommandEntry_t;

typedef CommandEntry_t *CommandEntry_pt;
typedef const CommandEntry_t *CommandEntry_cpt;

typedef struct {
    int fd; //!< file descriptor
    /*!
    \brief
        before read: character count to read,<br/>
        after read: character count actually read,<br/>
        before write: character count to write,<br/>
            if count == -1, whole specified string is written.<br/>
        after write: character count actually written.<br/>
    */
    int count;
    char buffer[64]; //!< I/O buffer
} IoParams_t;

typedef IoParams_t *IoParams_pt;
typedef const IoParams_t *IoParams_cpt;

typedef struct {
    int baud; //!< baudrate descriptor
    char devname[64]; //!< serialport device name
} OpenParams_t;

typedef OpenParams_t *OpenParams_pt;
typedef const OpenParams_t *OpenParams_cpt;

#if !defined(_MAIN_C)
extern char g_buffer[128];
extern IoParams_t g_IoParams;
extern OpenParams_t g_OpenParams;
#else
char g_buffer[128];
IoParams_t g_IoParams = { -1, 0, { '\0' } };
OpenParams_t g_OpenParams = { B1152000, {'\0'} };
#endif /* _MAIN_C */

#define G_BUFFER_SIZE     (sizeof(g_buffer)/sizeof(g_buffer[0]))

#define TEST_END(_err, _file, _func, _line) { \
    int __err = _err; \
    printf("Test ended with err = 0x%04x(%d) @ %s,%s,%d\n", \
    __err, __err, _file, _func, _line); }

#define IF_TEST_FAIL_BREAK(_err, _file, _func, _line, ...) { \
int __err = _err; \
if (__err) \
{ \
    printf("Test ended with err = 0x%04x(%d) @ %s,%s,%d\n", \
    __err, __err, _file, _func, _line); \
    printf(__VA_ARGS__); \
    break; \
} }

#endif /* _COMMON_H */