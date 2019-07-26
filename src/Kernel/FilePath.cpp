#include "FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    const FilePath & FilePath::none()
    {
        static FilePath s_none;

        return s_none;
    }    
}