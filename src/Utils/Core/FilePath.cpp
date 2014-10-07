#   include "FilePath.h"

#   include "Interface/StringizeInterface.h"

namespace Menge
{
    ConstString concatenationFilePath( ServiceProviderInterface * _serviceProvider, const ConstString & _left, const ConstString & _right )
    {
        PathString path;

		path += _left;
		path += _right;
       
        ConstString c_path = Helper::stringizeString( _serviceProvider, path );

        return c_path;
    }
}  