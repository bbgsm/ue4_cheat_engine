
#ifndef EP_IOUTILS_H
#define EP_IOUTILS_H

#include  <string>
#include  "Type.h"
#include  "IOInter.h"

using namespace std;


class IOUtils : public IOInter {
private:
    FILE *file_fd = nullptr;
    const char *file_path;
    mlong file_seek = 0;
public:
    IOUtils(const char *path, const char *flags);

    IOUtils(const char *path);

    ~IOUtils();

    bool isOpen();

    mlong setSeek(int64_t off);

    mlong getSeek();

    int read(void *buff, int len, off64_t offset = -1);

    int write(const void *buff, int len, off64_t offset = -1);

    int64_t getFileSize();

    static int64_t getFileSize(const char *path);

    void close();
};


#endif //EP_IOUTILS_H
