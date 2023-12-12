#define CRCCOMMON_C
#include "CRCCommon.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

const char* SkipSpace(const char* scanner)
{
    while (isspace(*scanner) && (*scanner != '\0')) scanner++;
    return scanner;
}

const char* SkipNonSpace(const char* scanner)
{
    while (!isspace(*scanner) && (*scanner != '\0')) scanner++;
    return scanner;
}

int Quit(void* out, const void* in)
{
    g_QuitRequest = 1;
    return EXIT_SUCCESS;
}

static const int BAUD_MAP[][2] =
{
    { 9600, B9600 },
    { 19200, B19200 },
    { 38400, B38400 },
    { 57600, B57600 },
    { 115200, B115200 },
    { 230400, B230400 },
};

int OpenSerial()
{
    int err = EXIT_SUCCESS;
    struct termios options;
    do {
        if (g_fd != -1)
        {
            err = EINVAL;
            break;
        }
        if (-1 == (g_fd = open(g_devname, O_RDWR | O_NOCTTY)))
        {
            err = errno;
            perror("Fail in OpenSerial()\n");
            break;
        }
        // change serialport parameters
        tcgetattr(g_fd, &options);
        cfsetispeed(&options, g_baud);
        cfsetospeed(&options, g_baud);
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
        tcsetattr(g_fd, TCSANOW, &options);
    } while (0);
    return err;
}

void CloseSerial()
{
    if (g_fd > 0)
    {
        close(g_fd);
        g_fd = -1;
    }
}

int SetCommParams(int argc, const char* argv[])
{
    int err = EXIT_SUCCESS;
    if (argc != 3)
    {
        fputs(COMMAND_HELP, stderr);
        exit(0);
    }    
    int baudrate = atoi(argv[1]);
    g_baud = -1;
    for (int index = 0; index < ARRAYSIZE(BAUD_MAP); index++)
    {
        if (baudrate == BAUD_MAP[index][0])
        {
            g_baud = BAUD_MAP[index][1];
            break;
        }
    }
    if (g_baud == -1)
    {
        fputs("Invalid baudrate\n", stderr);
        fputs(COMMAND_HELP, stderr);
        exit(0);
    }
    strcpy(g_devname, argv[2]);
    return OpenSerial();
}

int StartCommThread()
{
    int err = EXIT_SUCCESS;
    do {
        if (EXIT_SUCCESS != (err =
            pthread_create(g_CommThread, NULL, CommThread, g_CommThreadArg)))
        {
            perror("Fail in StartCommThread()");
            break;
        }
    } while (0);
    return err;
}


uint8_t ParseHex1Digit(const char* hexStr)
{
    uint8_t result = 0xff;
    uint8_t digitStr = hexStr[0];
    if ('0' <= digitStr && digitStr <= '9')
    {
        result = digitStr - '0';
    }
    else if ('A' <= digitStr && digitStr <= 'F')
    {
        result = digitStr - 'A' + 10;
    }
    else if ('a' <= digitStr && digitStr <= 'f')
    {
        result = digitStr - 'a' + 10;
    }
    return result;
}

uint16_t ParseHex2Digits(const char* hexStr)
{
    uint16_t result = 0xffff;
    uint8_t halfResultUpper = ParseHex1Digit(hexStr[0]);
    if (halfResultUpper > 0x0f) return result;
    uint8_t halfResultLower = ParseHex1Digit(hexStr[1]);
    if (halfResultLower > 0x0f) return result;
    result = halfResultLower + halfResultUpper << 4;
    return result;
}

void ToHexHalf(char* buffer, uint8_t u8half)
{
    *buffer = (u8half < 10) ? u8half + '0' : u8half + 'A' - 10;
}

void ToHexString(char* buffer, uint8_t u8)
{
    uint8_t half = u8 >> 4;
    ToHexHalf(buffer, half);
    half = u8 & 0x0f;
    ToHexHalf(buffer + 1, half);
}