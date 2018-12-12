#include "NodeDebuggerApp.h"

int main()
{
    NodeDebuggerApp app;
    if( app.Initialize() )
    {
        app.Loop();
        app.Shutdown();
    }

    return 0;
}
