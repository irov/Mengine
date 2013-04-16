#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Core/ConstString.h"

#   include <vector>

namespace Menge
{
    typedef ConstString FilePath; //UTF8

    typedef std::vector<FilePath> TVectorFilePath;

    FilePath concatenationFilePath( ServiceProviderInterface * _serviceProvider, const FilePath & _left, const FilePath & _right );
}  