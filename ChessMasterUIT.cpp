#include <SFML/Graphics.hpp>
#include <iostream>
#include "Application.h"

#ifdef _WIN32
#include <windows.h>
// Disable DPI scaling for Windows
void DisableDPIScaling() {
    // Try Windows 10 1703+ method
    typedef BOOL(WINAPI* SetProcessDpiAwarenessContextFunc)(DPI_AWARENESS_CONTEXT);
    HMODULE user32 = LoadLibraryA("user32.dll");
    if (user32) {
        SetProcessDpiAwarenessContextFunc setDpiAwarenessContext = 
            (SetProcessDpiAwarenessContextFunc)GetProcAddress(user32, "SetProcessDpiAwarenessContext");
        if (setDpiAwarenessContext) {
            setDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            std::cout << "[DPI] Set DPI awareness to PerMonitorV2" << std::endl;
        }
        FreeLibrary(user32);
    }
    
    // Fallback: Try Windows 8.1+ method
    typedef HRESULT(WINAPI* SetProcessDpiAwarenessFunc)(int);
    HMODULE shcore = LoadLibraryA("shcore.dll");
    if (shcore) {
        SetProcessDpiAwarenessFunc setProcessDpiAwareness = 
            (SetProcessDpiAwarenessFunc)GetProcAddress(shcore, "SetProcessDpiAwareness");
        if (setProcessDpiAwareness) {
            // PROCESS_PER_MONITOR_DPI_AWARE = 2
            setProcessDpiAwareness(2);
            std::cout << "[DPI] Set process DPI awareness (8.1+)" << std::endl;
        }
        FreeLibrary(shcore);
    }
}
#endif

int main() {
#ifdef _WIN32
    std::cout << "[Main] Disabling DPI scaling..." << std::endl;
    DisableDPIScaling();
#endif

    std::cout << "[Main] Creating Application..." << std::endl;
    Application app;
    app.run();
    return 0;
}
