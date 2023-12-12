#define CRC_C
#include "CRC.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>

#pragma region PRIMITIVE_FORMATTER
void ToBitStringU32(char* buf, uint32_t i)
{
    uint32_t mask = 0x80000000;
    for (int ib = 0; ib < 32; ib++, mask >>= 1)
    {
        *buf++ = (mask & i) ? '1' : '0';
    }
}

// Space padding for value 0 bits higher than the first value 1 bit.
void ToBitStringU32S(char* buf, uint32_t i)
{
    uint32_t mask = 0x80000000;
    char _0 = ' ';
    for (int ib = 0; ib < 32; ib++, mask >>= 1)
    {
        if ((mask & i) && (_0 == ' '))
        {
            _0 = '0';
        }
        *buf++ = (mask & i) ? '1' : _0;
    }
}

void ToBitStringU16(char* buf, uint16_t i)
{
    uint16_t mask = 0x8000;
    for (int ib = 0; ib < 16; ib++, mask >>= 1)
    {
        *buf ++ = (mask & i) ? '1' : '0';
    }
}

void ToBitStringU8(char* buf, uint8_t i)
{
    uint8_t mask = 0x80;
    for (int ib = 0; ib < 8; ib++, mask >>= 1)
    {
        *buf ++ = (mask & i) ? '1' : '0';
    }
}
#pragma endregion PRIMITIVE_FORMATTER
#pragma region CRC_DEBUGGING_AIDS
#define FORMAT_BUFFER_LEN   64
static char TraceBuffer[FORMAT_BUFFER_LEN];
#define TraceBuffer_CLEAR() memset(TraceBuffer, ' ', FORMAT_BUFFER_LEN);
// buffer structure
// [16] title [1] separator [32] CRC registers [1] separator [8] input byte
// [total] = 16 + 1 + 32 + 1 + 8 + 1 = 59 ~ align(8) => 64 bytes

#define OFFSET_TITLE    0
#define OFFSET_CRC_REGISTER 17
#define OFFSET_INPUT_BYTE   (OFFSET_CRC_REGISTER + 33)

#define TITLE_EMPTY     "                "
#define TITLE_INPUT     "     AFTER_INPUT"
#define TITLE_BEFORE    "BEFORE_ELIMINATE"
#define TITLE_AFTER     " AFTER_ELIMINATE"
#define TITLE_POLY      "      POLYNOMIAL"
#define CRC_HEADLINE    "FEDCBA9876543210FEDCBA9876543210 76543210\n"

void CRC_FormatRegisters(CRC_cpt crc)
{
    ToBitStringU32(TraceBuffer + OFFSET_CRC_REGISTER, crc->Shifter);
    ToBitStringU8(TraceBuffer + OFFSET_INPUT_BYTE, crc->Input);
    sprintf(TraceBuffer + OFFSET_INPUT_BYTE + 8, "\n");
}

void CRC_FormatPolynomial(CRC_cpt crc)
{
     ToBitStringU32S(TraceBuffer + OFFSET_CRC_REGISTER, crc->CRCPoly);
     sprintf(TraceBuffer + OFFSET_INPUT_BYTE, "\n");
}

void CRC_Dump(CRC_cpt crc, CRC_DumpType_t dt)
{
    TraceBuffer_CLEAR();
    switch (dt)
    {
        case CRC_DumpType_AFTER_INPUT:
            memcpy(TraceBuffer, TITLE_INPUT, strlen(TITLE_INPUT));
            CRC_FormatRegisters(crc);
            break;
        case CRC_DumpType_BEFORE_ELIMINATION:
            memcpy(TraceBuffer, TITLE_BEFORE, strlen(TITLE_BEFORE));
            CRC_FormatRegisters(crc);
            break;
        case CRC_DumpType_AFTER_ELIMINATION:
            memcpy(TraceBuffer, TITLE_AFTER, strlen(TITLE_AFTER));
            CRC_FormatRegisters(crc);
            break;
        case CRC_DumpType_POLYNOMIAL:
            memcpy(TraceBuffer, TITLE_POLY, strlen(TITLE_POLY));
            CRC_FormatPolynomial(crc);
            break;
        case CRC_DumpType_HEADLINE:
            memcpy(TraceBuffer, TITLE_EMPTY, strlen(TITLE_EMPTY));
            strcpy(TraceBuffer + OFFSET_CRC_REGISTER, CRC_HEADLINE);
            break;
    }
    fputs(TraceBuffer, stdout);
}

#ifdef CRC_TRACE_ON
#define CRC_TRACE(_crc, _dt) CRC_Dump(_crc, _dt)
#else
#define CRC_TRACE(_crc, _dt)
#endif
#pragma endregion CRC_DEBUGGING_AIDS

// MSB Bit masks of typical size integers
#define BM_MSB32        0x80000000
#define BM_MSB16        0x8000
#define BM_MSB8         0x80

void CRC_Put(CRC_pt crc, uint8_t b)
{
    int32_t mask = BM_MSB8;
    CRC_TRACE(crc, CRC_DumpType_POLYNOMIAL);
    crc->Input = (int32_t)b;
    CRC_TRACE(crc, CRC_DumpType_AFTER_INPUT);
    for (int ib = 0; ib < 8; ib++)
    {
        // move Input MSB to Shifter LSB.
        crc->Shifter = (crc->Shifter << 1) | ((mask & crc->Input) ? 1 : 0);
        crc->Input <<= 1;
        CRC_TRACE(crc, CRC_DumpType_BEFORE_ELIMINATION);
        // eliminate the shifter bit corresponding to CRC generator polynomial MSB
        // if they match; i.e. The shifter bit value is 1.
        if (crc->Shifter & crc->MSBMask)
        {
            crc->Shifter ^= crc->CRCPoly;
            CRC_TRACE(crc, CRC_DumpType_AFTER_ELIMINATION);
        }
    }
}

/*!
\brief Put a byte string.
*/
void CRC_Puts(CRC_pt crc, const uint8_t* b, int32_t count)
{
    for (int32_t i = 0; i < count; i++)
    {
        CRC_Put(crc, b[i]);
    }
}

void CRC_Put0s(CRC_pt crc, int32_t count)
{
    for (int32_t i = 0; i < count; i++)
    {
        CRC_Put(crc, 0);
    }
}

#pragma region REVERT_BIT_ORDER
void RevertBitOrder_Init()
{
    for (int32_t i = 0; i < 256; i++)
    {
        int32_t mask = BM_MSB8;
        int32_t setBit = 1;
        uint8_t work = 0;
        for (int32_t j = 0; j < 8; j++, mask >>= 1, setBit <<= 1)
        {
            work |= (mask & i) ? setBit : 0;
        }
        RevertBitOrder[i] = work;
    }
}

void RevertBitOrder_Apply(uint8_t* array, int32_t count)
{
    for (int32_t i = 0; i < count; i++)
    {
        array[i] = RevertBitOrder[array[i]];
    }
}

void RevertBitOrder_Convert
(uint8_t* dst, const uint8_t* src, int32_t count)
{
    for (int32_t i = 0; i < count; i++)
    {
        dst[i] = RevertBitOrder[src[i]];
    }
}
#pragma endregion REVERT_BIT_ORDER