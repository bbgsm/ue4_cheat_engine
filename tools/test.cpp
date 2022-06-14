
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <list>
#include <utility>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include "Type.h"


using namespace std;


void s() {
    printf("long:%d\n", sizeof(long));
    printf("int:%d\n", sizeof(int));
    printf("mlong:%d\n", sizeof(mlong));
    printf("int64_t:%d\n", sizeof(int64_t));
    printf("byte:%d\n", sizeof(mbyte));
    printf("float:%d\n", sizeof(float));
    printf("double:%d\n", sizeof(double));
    printf("long double:%d\n", sizeof(long double));
    printf("char:%d\n", sizeof(char));
    printf("unsigned char:%d\n", sizeof(unsigned char));
    printf("Byte:%d\n", sizeof(signed char));
}


int main() {

    s();
    return 1;
}
