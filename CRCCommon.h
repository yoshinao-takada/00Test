#if !defined(CRCCOMMON_H)
#define CRCCOMMON_H
#include "common.h"
#include <pthread.h>
#if defined(CRCCOMMON_C)
#define COMMONVAR
#else
#define COMMONVAR   extern
#endif
COMMONVAR int g_fd = -1; // file descriptor of a serialport
COMMONVAR int g_QuitRequest = 0; // quit request flag commanding main thread.
COMMONVAR int g_baud = 0; // baudrate
COMMONVAR char g_devname[32]; // devicename
COMMONVAR pthread_t* g_CommThread;
COMMONVAR void* g_CommThreadArg;

/*!
\brief skip whitespace characters in a string pointed by scanner.
*/
const char* SkipSpace(const char* scanner);

/*!
\brief skip non-whitespace characters in a string pointed by scanner.
*/
const char* SkipNonSpace(const char* scanner);

/*!
\brief Client/Server "q"uit command handlder
\param out [out] dummy arg
\param in [in] dummy arg
\return always EXIT_SUCCESS
*/
int Quit(void* out, const void* in);

/*!
\brief Set communication parameters, reading commandline.
*/
int SetCommParams(int argc, const char* argv[]);

#define COMMAND_HELP "How to invoke:\n" \
"\tCRCCli/CRCSvr  <baudrate>  <device_name>\n" \
"\t<baudrate>: a baudrate like 9600, 19200, 115200, etc. one of standard baudrates.\n" \
"\t<device_name>: one of serialport devices like /dev/ttyUSB0\n"

int OpenSerial();
void CloseSerial();

int StartCommThread();

void* CommThread(void* params);

/*!
\brief parse a hexadecimal digit.
\return 0x00-0x0f: success, 0xff: failure
*/
uint8_t ParseHex1Digit(const char* hexStr);

/*!
\brief parse two hexadecimal digits
\return 0x00-0xff: success, 0xffff: failure
*/
uint16_t ParseHex2Digits(const char* hexStr);

/*!
\brief convert 4 bit uint to a hexadecimal digit.
*/
void ToHexHalf(char* buffer, uint8_t u8half);

/*!
\brief convert an 8 bit uint to hexadecimal two digits.
*/
void ToHexString(char* buffer, uint8_t u8);
#endif /* CRCCOMMON_H */