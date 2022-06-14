rd /s/q build_%1
mkdir build_%1 && cd build_%1
cmake -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=D:/NDK/android-ndk-r20b/build/cmake/android.toolchain.cmake ^
	-DANDROID_ABI="%1" ^
	-DANDROID_STL="c++_static" ^
	-DCMAKE_BUILD_TYPE=Release ^
	-DANDROID_NDK=D:/NDK/android-ndk-r20b ^
	-DANDROID_PLATFORM=android-26 ^
	../../
mingw32-make -j14
cd ..
