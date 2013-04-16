#   include "FilePath.h"

#   include "Interface/StringizeInterface.h"

namespace Menge
{
    FilePath concatenationFilePath( ServiceProviderInterface * _serviceProvider, const FilePath & _left, const FilePath & _right )
    {
        static String s_buff;

        s_buff.assign( _left.c_str(), _left.size() );
        s_buff.append( _right.c_str(), _right.size() );
        
        FilePath path = Helper::stringizeString( _serviceProvider, s_buff );

        return path;
    }
}  