#include "common.h"
#include "commands.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/*/
\brief Break if _cond != false
\param _errPtr [out] pointer to error code variable.
\param _out [in] output stream FILE pointer
\param _cond [in] condition equiation or variable
\param _msg [in] error message
*/
#define COMMAND_IF_BADFORMAT(_errPtr, _out, _cond, _msg) if (_cond) { \
    fputs(_msg, _out); *_errPtr = EBADF; break; }

#define COMMAND_IF_INVALID(_errPtr, _out, _cond, _msg) if (_cond) { \
    fputs(_msg, _out); *_errPtr = EINVAL; break; }

static const int BAUD_MAP[][2] = {
    // defined in termios.h
    { 110, B110 },
    { 150, B150 },
    { 300, B300 },
    { 600, B600 },
    { 1200, B1200 },
    { 2400, B2400 },
    { 4800, B4800 },
    { 9600, B9600 },
    { 19200, B19200 },
    { 38400, B38400 },
    // defined in termios-baud.h
    { 57600, B57600 },
    { 115200, B115200 },
    // .. and more
};

static const int BAUD_COUNT = ARRAYSIZE(BAUD_MAP);

static const char* SkipSpace(const char* scanner)
{
    while (isspace(*scanner) && (*scanner != '\0')) scanner++;
    return scanner;
}

static const char* SkipNonSpace(const char* scanner)
{
    while (!isspace(*scanner) && (*scanner != '\0')) scanner++;
    return scanner;
}

static int NumberToBaudrateDescriptor(int number) 
{
    int baudDesc = -1;
    for (int i = 0; i < BAUD_COUNT; i++)
    {
        if (BAUD_MAP[i][0] == number)
        {
            baudDesc = BAUD_MAP[i][1];
            break;
        }
    }
    return baudDesc;
}

/*!
\brief command format is<br/>
<command>  <baudrate-number>  <device-name><br/>
<command>: command itself is not treated by the parser.<br/>
<badurate-number>: baudrate number like "110", "150", "300", etc.<br/>
<device-name>: serialport device name like
    "/dev/ttyS0", "dev/ttyUSB0", etc.<br/>
*/
int MyOpenBlkParser(void* out, const char* in)
{
    const long min_baud = (long)BAUD_MAP[0][0];
    const long max_baud = (long)BAUD_MAP[BAUD_COUNT-1][0];
    int err = EXIT_SUCCESS;
    const char *scanner = in;
    char* wordEnd = NULL;
    long baudrate = -1;
    OpenParams_pt params = (OpenParams_pt)out;
    do
    {
        scanner = SkipNonSpace(scanner); // skip command
        
        // get baudrate
        baudrate = strtol(scanner, &wordEnd, 10);
        COMMAND_IF_BADFORMAT(&err, stderr,
           (baudrate < min_baud && max_baud < baudrate),
            "Invalid baudrate. Maybe range error.\n");
        baudrate = NumberToBaudrateDescriptor(baudrate);
        COMMAND_IF_BADFORMAT(&err, stderr,
            (baudrate == -1), "Invalid baudrate. Not in available options.\n");
        params->baud = baudrate;

        // skip whitespace upto other characters
        scanner = SkipSpace(wordEnd);
        COMMAND_IF_BADFORMAT(&err, stderr,
            (*scanner == '\0'), "No device name was found.\n");
        COMMAND_IF_BADFORMAT(&err, stderr,
            (strlen(scanner) > (ARRAYSIZE(params->devname) - 1)),
            "Too long device name.\n");
        wordEnd = (char*)SkipNonSpace(scanner);
        strncpy(params->devname, scanner, wordEnd - scanner);
    }
    while (0);
    return err;
}

int MyOpenNBlkParser(void* out, const char* in)
{
    return MyOpenBlkParser(out, in);
}

int WriteStringParser(char* out, const char* in)
{
    int err = EXIT_SUCCESS;
    int escaped = 0;
    for (; *in != '\0'; *in++)
    {
        if (escaped == 0)
        {
            if (*in == '\\')
            {
                escaped++;
                in;
            }
            else
            {
                *out++ = *in;
            }
        }
        else
        {
            if (*in == '\\')
            {
                *out++ = '\\';
                escaped--;
            }
            else if (*in == 'r')
            {
                *out++ = '\r';
                escaped--;
            }
            else if (*in == 'n')
            {
                *out++ = '\n';
                escaped--;
            }
            else
            {
                err = EBADF;
                break;
            }
        }
    }
    *out = '\0';
    return err;
}

int MyWriteParser(void* out, const char* in)
{
    int err = EXIT_SUCCESS;
    const char *scanner = in;
    char* wordEnd = NULL;
    long count = -1000;
    IoParams_pt params = (IoParams_pt)out;
    char* writer = params->buffer;
    do
    {
        scanner = SkipNonSpace(scanner); // skip command
        count = strtol(scanner, &wordEnd, 10);
        COMMAND_IF_BADFORMAT(&err, stderr, count < -1,
            "Invalid character count to write.\n");
        COMMAND_IF_BADFORMAT(&err, stderr, *wordEnd == '\0',
            "No string to write was found.\n");
        params->count = (int)count;
        scanner = SkipSpace(wordEnd); // skip spaces upto output string
        COMMAND_IF_BADFORMAT(&err, stderr, *scanner == '\0',
            "No output string found.\n");
        COMMAND_IF_BADFORMAT(&err, stderr, 
            strlen(scanner) >= ARRAYSIZE(params->buffer),
            "Too long write string.\n");
        err = WriteStringParser(params->buffer, scanner);
    }
    while (0);
    return err;
}

int MyReadParser(void* out, const char* in)
{
    int err = EXIT_SUCCESS;
    const char *scanner = in;
    char *wordEnd = NULL;
    long count = -1000;
    IoParams_pt params = (IoParams_pt)out;
    do
    {
        scanner = SkipNonSpace(scanner); // skip command
        count = strtol(scanner, &wordEnd, 10);
        COMMAND_IF_BADFORMAT(&err, stderr, count < -100,
            "Invalid character count to write.\n");
        params->count = (int)count;
    }
    while (0);
    return err;
}

int NoneParser(void* out, const char* in)
{
    return EXIT_SUCCESS;
}

int MyClose(void* out, const void* in)
{
    int* quitRequest = (int*)out;
    *quitRequest = 1;
}

int MyOpenBlk(void* out, const void* in)
{
    int err = EXIT_SUCCESS;
    OpenParams_cpt openParams = (OpenParams_cpt)in;
    int* fd = (int*)out;
    struct termios options;
    do
    {
        *fd = open(openParams->devname, O_RDWR | O_NOCTTY);
        COMMAND_IF_INVALID(&err, stderr, *fd < 0, "Maybe invalid device name.\n");

        // change baudrate
        tcgetattr(*fd, &options);
        cfsetispeed(&options, openParams->baud);
        cfsetospeed(&options, openParams->baud);
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
        options.c_cflag &= ~(IXON | IXOFF | IXANY);
        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        options.c_iflag &= ~(INLCR | ICRNL);
        options.c_oflag &= ~OPOST;
        // It is necessary to set VMIN when VTIME works.
        // The timeout parameter is used only in Non-canonical and blocking mode.
        options.c_cc[VMIN] = 0;
        options.c_cc[VTIME] = 50; // 50 * (1/10) seconds = 5.0 seconds
        tcsetattr(*fd, TCSANOW, &options);
    }
    while (0);
    TEST_END(err,__FILE__, __FUNCTION__, __LINE__);
    perror("This error is: ");
    return err;
}

int MyOpenNBlk(void* out, const void* in)
{
    int err = EXIT_SUCCESS;
    OpenParams_cpt openParams = (OpenParams_cpt)in;
    int* fd = (int*)out;
    struct termios options;
    do
    {
        *fd = open(openParams->devname, O_RDWR | O_NONBLOCK | O_NOCTTY);
        COMMAND_IF_INVALID(&err, stderr, *fd < 0, "Maybe invalid device name.\n");

        // change baudrate
        tcgetattr(*fd, &options);
        cfsetispeed(&options, openParams->baud);
        cfsetospeed(&options, openParams->baud);
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
        options.c_iflag &= ~(IXON | IXOFF | IXANY);
        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        options.c_iflag &= ~(INLCR | ICRNL);
        options.c_oflag &= ~OPOST;
        tcsetattr(*fd, TCSANOW, &options);
    }
    while (0);
    TEST_END(err,__FILE__, __FUNCTION__, __LINE__);
    perror("This error is: ");
    return err;
}

int MyRead(void* out, const void* in)
{
    int err = EXIT_SUCCESS;
    IoParams_pt params = (IoParams_pt)in;
    int readCount = -1;
    char* outbuf = (char*)out;
    do
    {
        readCount = read(params->fd, params->buffer, params->count);
        COMMAND_IF_INVALID(&err, stderr, readCount < 0,
            "read error.\n");
        for (int i = 0; i < readCount; i++)
        {
            if (params->buffer[i] < ' ')
            {
                *outbuf++ = '.';
            }
            else
            {
                *outbuf++ = params->buffer[i];
            }
        }
        char* fmt = "[%02x";
        for (int i = 0; i < readCount; i++)
        {
            outbuf += sprintf(outbuf, fmt, params->buffer[i]);
            fmt = " %02x";            
        }
        sprintf(outbuf, "]%c", '\0');
        printf("readCount = %d\nread string = %s\n",
            readCount, (const char*)out);
        params->count = readCount;
    }
    while (0);
    if (err)
    {
        printf("errno = 0x%04x(%d) @ %s,%s,%d\n",
            errno, errno, __FILE__, __FUNCTION__, __LINE__);
    }
    TEST_END(err,__FILE__, __FUNCTION__, __LINE__);
    perror("This error is: ");
    return err;
}

int MyWrite(void* out, const void* in)
{
    int err = EXIT_SUCCESS;
    IoParams_cpt params = (IoParams_pt) in;
    ssize_t writtenCount = 0;
    size_t writeCount = -1;
    do
    {
        writeCount = (params->count > 0) ?
            params->count : strlen(params->buffer);
        writtenCount = write(params->fd, params->buffer, writeCount);
        COMMAND_IF_INVALID(&err, stderr, writtenCount < 0,
            "write error.\n");
        printf("writtenCount = %d\n", writeCount);
    }
    while (0);
    if (err)
    {
        printf("errno = 0x%04x(%d) @ %s,%s,%d\n",
            errno, errno, __FILE__, __FUNCTION__, __LINE__);
    }
    TEST_END(err,__FILE__, __FUNCTION__, __LINE__);
    perror("This error is: ");
    return err;
}

int MyQuit(void* out, const void* in)
{
    int* quitRequest = (int*)out;
    *quitRequest = 1;
    return EXIT_SUCCESS;    
}

static const char HELP_MESSAGE[] =
"c : close a port currently opened.\n"
"ob: open a port in blocking mode; e.g. o0 19200 /dev/ttyUSB0\n"
"onb: open a port in nonblocking mode; e.g. o1 9600 /dev/ttyS0\n"
"r: read a port currently opened; e.g. r <char count to read = 1..63>\n"
"w: write a string into a port currently opened;\n"
"\tex.1: w 0 abc012 (write a string, \"abc012\")\n"
"q: quit the program.\n"
"h: show this help message.\n";

int MyHelp(void* out, const void* in)
{
    fputs(HELP_MESSAGE, stdout);
    return EXIT_SUCCESS;
}

int NoneCommand(void* out, const void* in)
{
    fputs("Your input looked invalid. Please reexamine.\n---\n", stdout);
    fputs((char*)in, stdout);
    fputs("---\n", stdout);
    return EXIT_SUCCESS;
}