#include "CRC.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

const uint8_t DATA[] = 
{
    0xff, 0x55, 0xaa, 0x00,
    0xff, 0x55, 0xaa, 0x01
};

const uint8_t BIT_ERROR_DATA[] =
{
    0xff, 0x54, 0xaa, 0x00,
    0xff, 0x55, 0xaa, 0x01
};

const uint8_t REVERTED_DATA[] =
{
    0xff, 0xaa, 0x55, 0x00,
    0xff, 0xaa, 0x55, 0x80
};

int RevertBitOrderUT()
{
    int err = EXIT_SUCCESS;
    uint8_t buffer[ARRAYSIZE(DATA)];
    memcpy(buffer, DATA, ARRAYSIZE(DATA));
    do {
        RevertBitOrder_Init();
        RevertBitOrder_Apply(buffer, ARRAYSIZE(buffer));
        for (int i = 0; i < ARRAYSIZE(buffer); i++)
        {
            err = (buffer[i] != REVERTED_DATA[i]) ? EINVAL : EXIT_SUCCESS;
            IF_TEST_FAIL_BREAK(err, __FILE__, __FUNCTION__, __LINE__,
                "Fail in RevertBitOrder_Apply()");
        }
        if (err) break;
        RevertBitOrder_Convert(buffer, REVERTED_DATA, ARRAYSIZE(buffer));
        for (int i = 0; i < ARRAYSIZE(buffer); i++)
        {
            err = (buffer[i] != DATA[i]) ? EINVAL : EXIT_SUCCESS;
            IF_TEST_FAIL_BREAK(err, __FILE__, __FUNCTION__, __LINE__,
                "Fail in RevertBitOrder_Convert()");
        }
    } while (0);
    TEST_END(err, __FILE__, __FUNCTION__, __LINE__);
    return err;
}

int CRC8UT()
{
    int err = EXIT_SUCCESS;
    CRC_t crc = CRC_INIT_CCITT8(0);
    uint8_t crcCode = 0;
    uint8_t bitFormatBuffer[9];
    bitFormatBuffer[8] = '\0';
    do {
        #ifdef CRC_TRACE_ON
        CRC_Dump(&crc, CRC_DumpType_HEADLINE);
        #endif
        for (int i = 0; i < ARRAYSIZE(DATA); i++)
        {
            CRC_Put(&crc, DATA[i]);
        }
        CRC_Put0s(&crc, 1);
        crcCode = (uint8_t)(0xff & crc.Shifter);
        #ifdef CRC_TRACE_ON
        ToBitStringU8(bitFormatBuffer, crcCode);
        printf("crcCode = %s\n", bitFormatBuffer);
        #endif
        crc.Shifter = 0;
        CRC_Puts(&crc, DATA, ARRAYSIZE(DATA));
        CRC_Put(&crc, crcCode);
        err = ((crc.Shifter & 0xff) == 0)? EXIT_SUCCESS : EINVAL;
        IF_TEST_FAIL_BREAK(err, __FILE__, __FUNCTION__, __LINE__,
            "Fail in CRC_Put\n");
        crc.Shifter = 0;
        CRC_Puts(&crc, BIT_ERROR_DATA, ARRAYSIZE(BIT_ERROR_DATA));
        CRC_Put(&crc, crcCode);
        err = ((crc.Shifter & 0xff) != 0) ? EXIT_SUCCESS : EINVAL;
        IF_TEST_FAIL_BREAK(err, __FILE__, __FUNCTION__, __LINE__,
            "Fail in CRC_Put\n");
        #ifdef CRC_TRACE_ON
        crcCode = (uint8_t)(0xff & crc.Shifter);
        ToBitStringU8(bitFormatBuffer, crcCode);
        printf("crcRemainder = %s\n", bitFormatBuffer);
        #endif
    } while (0);
    TEST_END(err, __FILE__, __FUNCTION__, __LINE__);
    return err;
}

int CRC16UT()
{
    int err = EXIT_SUCCESS;
    CRC_t crc = CRC_INIT_CCITT16_BR(0);
    union {
        uint16_t u16;
        uint8_t u8[2];
    } crcCode = {0};
    uint8_t bitFormatBuffer[17];
    bitFormatBuffer[16] = '\0';
    do {
        #ifdef CRC_TRACE_ON
        CRC_Dump(&crc, CRC_DumpType_HEADLINE);
        #endif
        for (int i = 0; i < ARRAYSIZE(DATA); i++)
        {
            CRC_Put(&crc, DATA[i]);
        }
        CRC_Put0s(&crc, 2);
        crcCode.u16 = (uint16_t)(0xffff & crc.Shifter);
        #ifdef CRC_TRACE_ON
        ToBitStringU16(bitFormatBuffer, crcCode.u16);
        printf("crcCode = %s\n", bitFormatBuffer);
        #endif
        crc.Shifter = 0;
        CRC_Puts(&crc, DATA, ARRAYSIZE(DATA));
        CRC_Put(&crc, crcCode.u8[1]);
        CRC_Put(&crc, crcCode.u8[0]);
        err = ((crc.Shifter & 0xffff) == 0) ? EXIT_SUCCESS : EINVAL;
        IF_TEST_FAIL_BREAK(err, __FILE__, __FUNCTION__, __LINE__,
            "Fail in CRC_Put\n");
        crc.Shifter = 0;
        CRC_Puts(&crc, BIT_ERROR_DATA, ARRAYSIZE(BIT_ERROR_DATA));
        CRC_Put(&crc, crcCode.u8[1]);
        CRC_Put(&crc, crcCode.u8[0]);
        err = ((crc.Shifter & 0xffff) != 0) ? EXIT_SUCCESS : EINVAL;
        IF_TEST_FAIL_BREAK(err, __FILE__, __FUNCTION__, __LINE__,
            "Fail in CRC_Put\n");
        #ifdef CRC_TRACE_ON
        crcCode.u16 = (uint16_t)(0xffff & crc.Shifter);
        ToBitStringU16(bitFormatBuffer, crcCode.u16);
        printf("crcRemainder = %s\n", bitFormatBuffer);
        #endif
    } while (0);
    TEST_END(err, __FILE__, __FUNCTION__, __LINE__);
    return err;
}

int CRC16TISample() 
{
    int err = EXIT_SUCCESS;
    const uint8_t TI_TEST_DATA[] = "123456789";
    uint8_t TI_TEST_DATA_BR[9];
    CRC_t crc = CRC_INIT_CCITT16_BR(0xffff);
    uint16_t crcCode = 0;
    do {
        RevertBitOrder_Init();
        RevertBitOrder_Convert
        (TI_TEST_DATA_BR, TI_TEST_DATA, strlen(TI_TEST_DATA));
        CRC_Puts(&crc, TI_TEST_DATA, strlen(TI_TEST_DATA));
        crcCode = (uint16_t)(0xffff & crc.Shifter);
        printf("crcCode = 0x%04x\n", crcCode);
        CRC_Put0s(&crc, 2);
        crcCode = (uint16_t)(0xffff & crc.Shifter);
        printf("crcCode = 0x%04x\n", crcCode);
    } while (0);
    TEST_END(err, __FILE__, __FUNCTION__, __LINE__);
    return err;
}

int CRC16MCPSample() 
{
    int err = EXIT_SUCCESS;
    const uint8_t data = 0x51;
    CRC_t crc = CRC_INIT_CCITT16_BR(0);
    do {
        CRC_Put(&crc, data);
        CRC_Put0s(&crc, 2);
    } while (0);
    TEST_END(err, __FILE__, __FUNCTION__, __LINE__);
    return err;
}
int main()
{
    int err = EXIT_SUCCESS;
    do {
        // IF_TEST_FAIL_BREAK(RevertBitOrderUT(),
        //     __FILE__, __FUNCTION__, __LINE__, "Fail in RevertBtOrdersUT()\n");
        // IF_TEST_FAIL_BREAK(CRC8UT(),
        //     __FILE__, __FUNCTION__, __LINE__, "Fail in CRC8UT()\n");
        // IF_TEST_FAIL_BREAK(CRC16UT(),
        //     __FILE__, __FUNCTION__, __LINE__, "Fail in CRC16UT()\n");
        IF_TEST_FAIL_BREAK(CRC16TISample(),
            __FILE__, __FUNCTION__, __LINE__, "Fail in CRC16TISample()\n");
        // IF_TEST_FAIL_BREAK(CRC16MCPSample(),
        //     __FILE__, __FUNCTION__, __LINE__, "Fail in CRC16TISample()\n");
    } while (0);
    TEST_END(err, __FILE__, __FUNCTION__, __LINE__);
    return err;
}