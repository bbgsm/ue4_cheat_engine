rd /s/q build
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=D:/NDK/android-ndk-r20b/build/cmake/android.toolchain.cmake ^
	-DANDROID_ABI="arm64-v8a" ^
	-DANDROID_STL="c++_static" ^
	-DCMAKE_BUILD_TYPE=Release ^
	-DANDROID_NDK=D:/NDK/android-ndk-r20b ^
	-DANDROID_PLATFORM=android-26 ^
	../
make -j8
cd ..
