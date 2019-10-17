
#include "windows.h"

char dllVersionBuffer[16] = ""; // a private buffer

// Python Globals
HMODULE PyWin_DLLhModule = NULL;
const char *PyWin_DLLVersionString = dllVersionBuffer;

