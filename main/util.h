#ifndef UTIL_H
#define UTIL_H

#define LOBYTE(x) ((char*)(&(x)))[0]
#define HIBYTE(x) ((char*)(&(x)))[1]
#define SWAP_32(x) \
    ((((x)&0xff000000) >> 24) | (((x)&0x00ff0000) >>  8) | \
     (((x)&0x0000ff00) <<  8) | (((x)&0x000000ff) << 24))
#define WORD_SWAP(x) (((x & 0xff00) >> 8) | ((x & 0x00ff) << 8))

void delay(uint32_t);
void littleToBigEndian(uint32_t*, uint32_t);
int bigToLittleEndian(uint8_t*);
void wordSwap(uint16_t*, size_t);

#endif