#include "Kernel/ParamsHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        String getParam( const MapWParams & _params, const ConstString & _key, const Char * _default )
        {
            typename MapWParams::const_iterator it_found = _params.find( _key );

            if( it_found == _params.end() )
            {
                return _default;
            }

            const MapWParams::mapped_type & value = it_found->second;

            String utf8_value;
            Helper::unicodeToUtf8( value, &utf8_value );

            return utf8_value;
        }
        //////////////////////////////////////////////////////////////////////////
        WString getParam( const MapWParams & _params, const ConstString & _key, WString && _default )
        {
            typename MapWParams::const_iterator it_found = _params.find( _key );

            if( it_found == _params.end() )
            {
                return _default;
            }

            const MapWParams::mapped_type & value = it_found->second;

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}