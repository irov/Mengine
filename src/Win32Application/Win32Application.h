#pragma once

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32Application
    {
    public:
        Win32Application();
        ~Win32Application();

    public:
        bool initialize();
        void finalize();

    public:
        void loop();

    protected:
        HINSTANCE m_hInstance;
    };
}
