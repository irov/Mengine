#pragma once

#include "Config/Config.h"

extern "C"
{
    bool mengineDocumentHasFocus();
    bool mengineIsCoarsePointer();
    bool mengineHasTouchPoints();
    bool mengineIsOnline();
    bool mengineOpenUrl( const char * _url );
    bool mengineWriteClipboard( const char * _text );
    void mengineSetupCanvas();
    void mengineFocusCanvas();
    void mengineGetCanvasOrigin( double * _left, double * _top );
}
