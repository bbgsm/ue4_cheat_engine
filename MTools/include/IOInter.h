

#ifndef EP_IOINTER_H
#define EP_IOINTER_H

#include  <string>
#include  "Type.h"

using namespace std;


class IOInter {
public:

    virtual mlong setSeek(int64_t off) = 0;

    virtual mlong getSeek() = 0;

    virtual int read(void *buff, int len, off64_t offset = -1) = 0;

    virtual int write(const void *buff, int len, off64_t offset = -1) = 0;

    virtual int64_t getFileSize() = 0;

    virtual void close() = 0;
};


#endif //EP_IOINTER_H
