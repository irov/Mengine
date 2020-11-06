#pragma once

#include "Kernel/Resource.h"

namespace Mengine
{
    namespace Helper
    {
        const FilePath & getResourceFilePath( const ResourcePtr & _resource );
        const FilePath & getResourceFilePathByName( const ConstString & _resourceName );
    }
}