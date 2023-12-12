#if !defined(_COMMANDS_H)
#define _COMMANDS_H
#pragma region DATA_PROCESSING_METHODS
/*!
\brief close a serialport
\param out [out] pointer to struct a file descriptor (int)
\param in [in] pointer to struct a file descriptor (int)
*/
int MyClose(void* out, const void* in);

/*!
\brief open a serialport in blocking mode
\param out [out] pointer to a file descriptor (int)
\param in [in] pointer to struct { int baudrate, char devname[] }
*/
int MyOpenBlk(void* out, const void* in);

/*!
\brief open a serialport in nonblocking mode
\param out [out] pointer to a file descriptor (int)
\param in [in] pointer to struct { int baudrate, char devname[] }
*/
int MyOpenNBlk(void* out, const void* in);

/*!
\brief read characters from a serialport.
\param out [out] pointer to a char buffer
\param in [in] pointer to a file descriptor (int)
*/
int MyRead(void* out, const void* in);

/*!
\brief write characters into a serialport
\param NULL pointer
\param in [in] pointer to a struct { int fd, char* buffer }
*/
int MyWrite(void* out, const void* in);

/*!
\brief quit a program
\param out [out] pointer to int quitRequest flag (1: quit, 0:continue)
\param in [in] NULL pointer
*/
int MyQuit(void* out, const void* in);

/*!
\brief show help message
*/
int MyHelp(void* out, const void* in);

/*!
\brief do nothing but print (char*)in.
*/
int NoneCommand(void* out, const void* in);
#pragma endregion DATA_PROCESSING_METHODS
#pragma region COMMAND_PARSER
/*!
\brief parse a command string for MyOpenBlk
\param out [out] pointer to struct { int baud, char devname[] }
\param in [in] command line string
\return EXIT_SUCCESS: success, EBADF: fail
*/
int MyOpenBlkParser(void* out, const char* in);

/*!
\brief parse a command string for MyOpenBlk
\param out [out] pointer to struct { int baud, char devname[] }
\param in [in] command line string
\return EXIT_SUCCESS: success, EBADF: fail
*/
int MyOpenNBlkParser(void* out, const char* in);

/*!
\brief parse a command string for MyRead
\param out [out] pointer to struct 
    { int fd, int readCount, char buffer[] }
\param in [in] command line string
*/
int MyReadParser(void* out, const char* in);

/*!
\brief parse a command string for MyWrite
\param out [out] pointer to struct { int fd, int count, char* buffer }
\param in [in] command line string
\return EXIT_SUCCESS: success, EBADF: fail
*/
int MyWriteParser(void* out, const char* in);

/*!
\brief do nothing, just return EXIT_SUCCESS
\return always EXIT_SUCCESS.
*/
int NoneParser(void* out, const char* in);
#pragma endregion COMMAND_PARSER
#endif /* _COMMANDS_H */