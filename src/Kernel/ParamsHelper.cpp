#include "Kernel/ParamsHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        String getParam( const Params & _params, const ConstString & _key, const Char * _default )
        {
            typename Params::const_iterator it_found = _params.find( _key );

            if( it_found == _params.end() )
            {
                return _default;
            }

            const Params::mapped_type & value = it_found->second;

            String string_value;
            if( Helper::get( value, &string_value ) == false )
            {
                return _default;
            }

            return string_value;
        }
        //////////////////////////////////////////////////////////////////////////
        WString getParam( const Params & _params, const ConstString & _key, WString && _default )
        {
            typename Params::const_iterator it_found = _params.find( _key );

            if( it_found == _params.end() )
            {
                return _default;
            }

            const Params::mapped_type & value = it_found->second;

            WString wstring_value;
            if( Helper::get( value, &wstring_value ) == false )
            {
                return _default;
            }

            return wstring_value;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}