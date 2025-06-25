#ifndef QR_CONSTANTS_H
#define QR_CONSTANTS_H

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MAX_COORDS       1024   // maximum number of coordinates
#define MAX_MATRIX_SIDE  64     // maximum matrix side length (for drawMatrix)
#define EDGE_LENGTH      25     // fixed edge length for ModuleCanvas
#define MAX_RUNS         64     // maximum run segments in ColorRun
#define MAX_BLOCKS_BITS  1024   // maximum bits in CodewordEncoder blocks
#define MAX_CODEWORDS    128    // maximum codewords produced
#define MAX_ECC          16     // maximum ECC bytes
#define MAX_INPUT_SIZE   64     // maximum input size in bytes

#endif // QR_CONSTANTS_H