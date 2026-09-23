#ifndef DATATOVIDEO_FORMAT_HPP
#define DATATOVIDEO_FORMAT_HPP

#include <cstdint>
struct Header {
    char magic[4];
    uint64_t fileSize;
    char fileExtension[16];
    uint16_t density;
};

#endif // DATATOVIDEO_FORMAT_HPP