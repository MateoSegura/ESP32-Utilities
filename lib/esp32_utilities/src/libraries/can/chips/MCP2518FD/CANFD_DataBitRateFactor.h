//----------------------------------------------------------------------------------------------------------------------
// A CANFD driver
// by Pierre Molinaro

// This header is common to libraries
//        https://github.com/pierremolinaro/ACAN_T4
//        https://github.com/pierremolinaro/MCP2518FD
//
//----------------------------------------------------------------------------------------------------------------------

#ifndef ACANFD_DATA_BIT_RATE_FACTOR_DEFINED
#define ACANFD_DATA_BIT_RATE_FACTOR_DEFINED

//----------------------------------------------------------------------------------------------------------------------

#include <stdint.h>

//----------------------------------------------------------------------------------------------------------------------

enum class DataBitRateFactor : uint8_t {
    x1 = 1,
    x2 = 2,
    x3 = 3,
    x4 = 4,
    x5 = 5,
    x6 = 6,
    x7 = 7,
    x8 = 8,
    x9 = 9,
    x10 = 10
} ;

//----------------------------------------------------------------------------------------------------------------------

#endif
