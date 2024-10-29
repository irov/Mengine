#include "Kernel/ParamsHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ParamBool getParam( const Params & _params, const ConstString & _key, ParamBool _default )
        {
            typename Params::const_iterator it_found = _params.find( _key );

            if( it_found == _params.end() )
            {
                return _default;
            }

            const Params::mapped_type & value = it_found->second;

            ParamBool bool_value;
            Helper::get( value, &bool_value );

            return bool_value;
        }
        //////////////////////////////////////////////////////////////////////////
        ParamInteger getParam( const Params & _params, const ConstString & _key, ParamInteger _default )
        {
            typename Params::const_iterator it_found = _params.find( _key );

            if( it_found == _params.end() )
            {
                return _default;
            }

            const Params::mapped_type & value = it_found->second;

            ParamInteger int_value;
            Helper::get( value, &int_value );

            return int_value;
        }
        //////////////////////////////////////////////////////////////////////////
        void setParam( Params & _params, const ConstString & _key, ParamInteger _value )
        {
            _params[_key] = _value;
        }
        //////////////////////////////////////////////////////////////////////////
        ParamDouble getParam( const Params & _params, const ConstString & _key, ParamDouble _default )
        {
            typename Params::const_iterator it_found = _params.find( _key );

            if( it_found == _params.end() )
            {
                return _default;
            }

            const Params::mapped_type & value = it_found->second;

            ParamDouble double_value;
            Helper::get( value, &double_value );

            return double_value;
        }
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

            Helper::visit( value
                , [&string_value]( const ParamNull & _element )
            {
                MENGINE_UNUSED( _element );
            }
                , [&string_value]( const ParamBool & _element )
            {
                if( _element == true )
                {
                    string_value = "1";
                }
                else
                {
                    string_value = "0";
                }
            }
                , [&string_value]( const ParamInteger & _element )
            {
                Char buffer[256 + 1] = {'\0'};
                Helper::stringalized( _element, buffer, 256 );

                string_value.assign( buffer );
            }
                , [&string_value]( const ParamDouble & _element )
            {
                Char buffer[256 + 1] = {'\0'};
                Helper::stringalized( _element, buffer, 256 );

                string_value.assign( buffer );
            }
                , [&string_value]( const ParamString & _element )
            {
                string_value.assign( _element );
            }
                , [&string_value, _default]( const ParamWString & _element )
            {
                MENGINE_UNUSED( _element );

                string_value.assign( _default );
            }
                , [&string_value]( const ParamConstString & _element )
            {
                string_value.assign( _element.c_str(), _element.size() );
            }   
                , [&string_value, _default]( const FactorablePtr & _element )
            {
                MENGINE_UNUSED( _element );

                string_value.assign( _default );
            } );

            return string_value;
        }
        //////////////////////////////////////////////////////////////////////////
        WString getParam( const Params & _params, const ConstString & _key, const WChar * _default )
        {
            typename Params::const_iterator it_found = _params.find( _key );

            if( it_found == _params.end() )
            {
                return _default;
            }

            const Params::mapped_type & value = it_found->second;

            WString wstring_value;

            Helper::visit( value
                , [&wstring_value]( const ParamNull & _element )
            {
                MENGINE_UNUSED( _element );
            }
                , [&wstring_value]( const ParamBool & _element )
            {
                if( _element == true )
                {
                    wstring_value = L"1";
                }
                else
                {
                    wstring_value = L"0";
                }
            }
                , [&wstring_value]( const ParamInteger & _element )
            {
                WChar buffer[256 + 1] = {'\0'};
                Helper::stringalized( _element, buffer, 256 );

                wstring_value.assign( buffer );
            }
                , [&wstring_value]( const ParamDouble & _element )
            {
                WChar buffer[256 + 1] = {'\0'};
                Helper::stringalized( _element, buffer, 256 );

                wstring_value.assign( buffer );
            }
                , [&wstring_value, _default]( const ParamString & _element )
            {
                MENGINE_UNUSED( _element );

                wstring_value.assign( _default );
            }
                , [&wstring_value]( const ParamWString & _element )
            {
                wstring_value.assign( _element );
            }
                , [&wstring_value, _default]( const ParamConstString & _element )
            {
                MENGINE_UNUSED( _element );

                wstring_value.assign( _default );
            }
                , [&wstring_value, _default]( const FactorablePtr & _element )
            {
                MENGINE_UNUSED( _element );

                wstring_value.assign( _default );
            } );

            return wstring_value;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString getParam( const Params & _params, const ConstString & _key, const ConstString & _default )
        {
            typename Params::const_iterator it_found = _params.find( _key );

            if( it_found == _params.end() )
            {
                return _default;
            }

            const Params::mapped_type & value = it_found->second;

            ConstString const_string_value;
            Helper::get( value, &const_string_value );

            return const_string_value;
        }
        //////////////////////////////////////////////////////////////////////////
        FactorablePointer getParam( const Params & _params, const ConstString & _key, const FactorablePtr & _default )
        {
            typename Params::const_iterator it_found = _params.find( _key );

            if( it_found == _params.end() )
            {
                return _default;
            }

            const Params::mapped_type & value = it_found->second;

            FactorablePtr type_value;
            Helper::get( value, &type_value );

            return type_value;
        }
        //////////////////////////////////////////////////////////////////////////
        void setParam( Params & _params, const ConstString & _key, const FactorablePtr & _value )
        {
            _params[_key] = _value;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}