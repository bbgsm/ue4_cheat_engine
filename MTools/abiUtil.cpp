
#include "abiUtil.h"

int abiUtil::abi(int arm32, int arm64) {
#if defined(__arm__)
    return arm32;
#elif defined(__aarch64__)
    return arm64;
#else
    return arm32;
#endif
}
