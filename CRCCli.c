#include "common.h"
#include "CRC.h"
#include "CRCCommon.h"
char SendBuffer[16];
char RecvBuffer[16];

int CliSend(void* out, const void* in)
{
    int err = EXIT_SUCCESS;
    IoParams_pt params = (IoParams_pt)in;
    const char* sendParamStr = params->buffer;
    ssize_t sentCount = 0;
    do {
        strcpy(SendBuffer, "s ");
        strncpy(SendBuffer + 2, sendParamStr, 2);
        SendBuffer[4] = '\n';
        sentCount = (g_fd, SendBuffer, 5);
        if (sentCount != 5)
        {
            err = errno;
            perror("Fail in CliSend()\n");
            break;
        }
    } while (0);
    return err;
}

int CliSendBR(void* out, const void* in)
{
    int err = EXIT_SUCCESS;
    IoParams_pt params = (IoParams_pt)in;
    const char* sendParamStr = params->buffer;
    ssize_t sentCount = 0;
    uint8_t hexValue;
    do {
        strcpy(SendBuffer, "s ");
        hexValue = ParseHex2Digits(params->buffer);
        strncpy(SendBuffer + 2, sendParamStr, 2);
        SendBuffer[4] = '\n';
        sentCount = (g_fd, SendBuffer, 5);
        if (sentCount != 5)
        {
            err = errno;
            perror("Fail in CliSend()\n");
            break;
        }
    } while (0);
    return err;
}

void* CommThread(void* params)
{
    do {
    } while (g_QuitRequest == 0);
    return params;
}

int main(int argc, const char* argv[])
{
    int err = EXIT_SUCCESS;
    do {
        RevertBitOrder_Init();
        if (EXIT_SUCCESS != (err = SetCommParams(argc, argv))) break;
        if (EXIT_SUCCESS != (err = OpenSerial())) break;
    } while (0);
    CloseSerial();
    return err;
}