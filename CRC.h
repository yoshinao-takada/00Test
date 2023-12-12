#if !defined(CRC_H)
#define CRC_H
#include <stdint.h>
#pragma region PRIMITIVE_FORMATTER
/*!
\brief Forrmat uint32_t number into a sequence of '0' and '1'.
*/
void ToBitStringU32(char* buf, uint32_t i);
/*!
\brief Forrmat uint32_t number into a sequence of '0' and '1'.
    Bits upper the first '1' bit in i is replaced with ' ' instead
    of '0'.
*/
void ToBitStringU32S(char* buf, uint32_t i);
/*!
\brief Format uint16_t number into a sequence of '0' and '1'.
*/
void ToBitStringU16(char* buf, uint16_t i);
/*!
\brief Format uint8_t number into a sequence of '0' and '1'.
*/
void ToBitStringU8(char* buf, uint8_t i);
#pragma endregion PRIMITIVE_FORMATTER

typedef struct {
    uint32_t MSBMask;
    uint32_t Shifter;
    uint32_t CRCPoly;
    uint32_t Input;
} CRC_t;
typedef CRC_t *CRC_pt;
typedef const CRC_t *CRC_cpt;

// note 1:
//  ex.1: CCITT CRC8, CRCSize = 8
//  ex.2: CCITT CRC16, CRCSize = 16

/*!
\brief Put a byte.
*/
void CRC_Put(CRC_pt crc, uint8_t b);

/*!
\brief Put a byte string.
*/
void CRC_Puts(CRC_pt crc, const uint8_t* b, int32_t count);

/*!
\brief Put post zero paddings.
\param count [in] byte count of zero paddings; e.g. 2 for CRC-16, 1 for CRC-8.
*/
void CRC_Put0s(CRC_pt crc, int32_t count);

#define CCITT16_MSBMASK     0x10000
// CCITT16 bit reverse generator polynomial = x^16 + x^12 + x^5 + 1
// 0b 1 0001 0000 0010 0001 = 0x11021
#define CCITT16_CRCPOLY_BR  0x11021
// CCITT16 bit big-endian generator polynomial = x^16 + x^7 + x^4 + 1
// 0b 1 0000 1000 0001 0001 = 0x10811
#define CCITT16_CRCPOLY     0x10811

// CCITT8 generator polynomial = x^8 + x^2 + x + 1
// 0b 1 0000 0111
#define CCITT8_CRCPOLY  0x107
#define CCITT8_MSBMASK  0x100

#define CRC_INIT_CCITT16_BR(_seed) { CCITT16_MSBMASK, _seed, CCITT16_CRCPOLY_BR, 0 }
#define CRC_INIT_CCITT16(_seed) { CCITT16_MSBMASK, _seed, CCITT16_CRCPOLY, 0 }
#define CRC_INIT_CCITT8(_seed) { CCITT8_MSBMASK, _seed, CCITT8_CRCPOLY, 0 }

/*!
\brief Reverted bit order byte table.
[0] = 0b00000000
[1] = 0b10000000
[2] = 0b01000000
[3] = 0b11000000
...
[254] = 0b01111111
[255] = 0b11111111
*/
#if !defined(CRC_C)
extern
#else
uint8_t RevertBitOrder[256];
#endif /* CRC_C */

/*!
\brief Initialize RevertedBitOrder.
*/
void RevertBitOrder_Init();

/*!
\brief Revert bit orders in an array in-place.
*/
void RevertBitOrder_Apply(uint8_t* array, int32_t count);

/*!
\brief Reverting bit orders, convert a byte array to another array.
*/
void RevertBitOrder_Convert(uint8_t* dst, const uint8_t* src, int32_t count);

#pragma region DEBUGGING_AID
typedef enum {
    // just after byte input
    CRC_DumpType_AFTER_INPUT,
    // just before MSB elimination, Shifter and Input
    CRC_DumpType_BEFORE_ELIMINATION,
    // just after MSB elimination, Shifter and Input
    CRC_DumpType_AFTER_ELIMINATION, 
    // Generator polynomial
    CRC_DumpType_POLYNOMIAL,
    // Headline
    CRC_DumpType_HEADLINE,
} CRC_DumpType_t;

void CRC_Dump(CRC_cpt crc, CRC_DumpType_t dt);
#pragma endregion DEBUGGING_AID
#endif /* CRC_H */
