/*
 * Codes from SsageParuders[https://github.com/SsageParuders]
 * 代码由泓清提供
*/
#include <EGL/egl.h>
#include <GLES/gl.h>


struct MDisplayInfo {
    uint32_t width{0};
    uint32_t height{0};
    uint32_t orientation{0};
};

NativeWindowType
createNativeWindow(const char *surface_name, uint32_t screen_width, uint32_t screen_height, bool author);

MDisplayInfo getDisplayInfo();