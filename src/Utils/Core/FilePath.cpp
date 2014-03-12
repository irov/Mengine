#   include "FilePath.h"

#   include "Interface/StringizeInterface.h"

namespace Menge
{
    FilePath concatenationFilePath( ServiceProviderInterface * _serviceProvider, const FilePath & _left, const FilePath & _right )
    {
        PathString path;

		path += _left;
		path += _right;
       
        FilePath c_path = Helper::stringizeString( _serviceProvider, path );

        return c_path;
    }
}  