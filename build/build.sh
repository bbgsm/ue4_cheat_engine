#!/bin/bash
rm -rf build_$1
mkdir build_$1 && cd build_$1
cmake -G "Unix Makefiles" \
  -DCMAKE_TOOLCHAIN_FILE=/usr/local/android-ndk-r20b/build/cmake/android.toolchain.cmake \
	-DANDROID_ABI="$1" \
	-DANDROID_STL="c++_static" \
	-DCMAKE_BUILD_TYPE=Release \
	-DANDROID_NDK=/usr/local/android-ndk-r20b \
	-DANDROID_PLATFORM=android-26 \
   ../../
make -j18
cd ..
