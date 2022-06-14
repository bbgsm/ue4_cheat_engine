rd /s/q build
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=D:/MDK/android-ndk-r20b/build/cmake/android.toolchain.cmake ^
	-DANDROID_ABI="x86_64" ^
	-DANDROID_STL="c++_static" ^
	-DCMAKE_BUILD_TYPE=Release ^
	-DANDROID_NDK=D:/MDK/android-ndk-r20b ^
	-DANDROID_PLATFORM=android-26 ^
	..
make -j8
cd ..

@REM cmake -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=D:/MDK/android-ndk-r20b/build/cmake/android.toolchain.cmake
@REM -DANDROID_ABI="x86_64"
@REM -DANDROID_STL="c++_static"
@REM -DCMAKE_BUILD_TYPE=Release
@REM -DANDROID_NDK=D:/MDK/android-ndk-r20b
@REM -DANDROID_PLATFORM=android-26