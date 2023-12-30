#ifndef CROSSWORD_UTIL_H
#define CROSSWORD_UTIL_H

#define CEIL_DIV(a, b) (((a) + b - 1) / b)

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define BITARRAY_GET(arr, n) (((arr)[(n) >> 3] >> (7 - ((n) & 7))) & 1)

#endif //CROSSWORD_UTIL_H
