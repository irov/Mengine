#include "StringHelper.h"

#include "Kernel/Assertion.h"

#include "Kernel/Vector.h"
#include "Kernel/VectorString.h"
#include "Kernel/Stringstream.h"

#include "Config/StdIO.h"
#include "Config/StdArg.h"
#include "Config/StdString.h"

#include "Config/Algorithm.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        String stringFormat( const Char * _format, ... )
        {
            Char str[2048] = {'\0'};

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );

            int32_t size_vsnprintf = MENGINE_VSNPRINTF( str, 2048, _format, args );

            MENGINE_VA_LIST_END( args );

            MENGINE_ASSERTION_FATAL( size_vsnprintf >= 0, "invalid string format '%s'"
                , _format
            );

            if( size_vsnprintf == 0 )
            {
                return String();
            }

            return String( str, size_vsnprintf );
        }
        //////////////////////////////////////////////////////////////////////////
        void split( VectorString * const _outStrings, const String & _str, bool _trimDelims, const String & _delim )
        {
            uint32_t numSplits = 0;
            String::size_type start = 0;
            String::size_type pos = 0;

            do
            {
                pos = _str.find_first_of( _delim, start );

                if( pos == String::npos )
                {
                    _outStrings->emplace_back( _str.substr( start ) );

                    break;
                }
                else
                {
                    _outStrings->emplace_back( _str.substr( start, pos - start ) );

                    start = pos + 1;
                }

                if( _trimDelims == true )
                {
                    start = _str.find_first_not_of( _delim, start );
                }

                ++numSplits;

            } while( pos != String::npos );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, class C = Vector<T> >
        static void t_split2( C * const _outStrings, const T & _str, bool _trimDelims, const C & _delims )
        {
            typename T::size_type start = 0;
            typename T::size_type pos = 0;

            do
            {
                pos = T::npos;

                for( typename C::const_iterator
                    it = _delims.begin(),
                    it_end = _delims.end();
                    it != it_end;
                    ++it )
                {
                    const T & delim = *it;

                    typename T::size_type pos_delim = _str.find_first_of( delim, start );

                    if( pos > pos_delim )
                    {
                        pos = pos_delim;
                    }
                }

                if( pos == T::npos )
                {
                    _outStrings->emplace_back( _str.substr( start ) );

                    break;
                }
                else
                {
                    _outStrings->emplace_back( _str.substr( start, pos - start ) );

                    start = pos + 1;
                }

                if( _trimDelims == true )
                {
                    typename T::size_type pos_n = T::npos;

                    for( typename C::const_iterator
                        it = _delims.begin(),
                        it_end = _delims.end();
                        it != it_end;
                        ++it )
                    {
                        const T & delim = *it;

                        typename T::size_type pos_delim = _str.find_first_not_of( delim, start );

                        if( pos_n > pos_delim )
                        {
                            pos_n = pos_delim;
                        }
                    }

                    start = pos_n;
                }
            } while( pos != T::npos );
        }
        //////////////////////////////////////////////////////////////////////////
        void split2( VectorString * const _outStrings, const String & _str, bool _trimDelims, const VectorString & _delims )
        {
            t_split2( _outStrings, _str, _trimDelims, _delims );
        }
        //////////////////////////////////////////////////////////////////////////
        void u32split2( VectorU32String * const _outStrings, const U32String & _str, bool _trimDelims, const VectorU32String & _delims )
        {
            t_split2( _outStrings, _str, _trimDelims, _delims );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, class C = Vector<T> >
        static bool t_split3( C * const _outStrings, const T & _str, const C & _delims )
        {
            typename T::size_type start = 0;
            typename T::size_type pos = 0;

            do
            {
                pos = T::npos;

                for( typename C::const_iterator
                    it = _delims.begin(),
                    it_end = _delims.end();
                    it != it_end;
                    ++it )
                {
                    const T & delim = *it;

                    typename T::size_type pos_delim = _str.find_first_of( delim, start );

                    if( pos > pos_delim )
                    {
                        pos = pos_delim;
                    }
                }

                if( pos == T::npos )
                {
                    if( start != 0 )
                    {
                        if( start != _str.size() )
                        {
                            _outStrings->emplace_back( _str.substr( start ) );
                        }

                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    _outStrings->emplace_back( _str.substr( start, pos - start ) );

                    start = pos + 1;
                }
            } while( pos != T::npos );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool u32split3( VectorU32String * const _outStrings, const U32String & _str, const VectorU32String & _delims )
        {
            return t_split3( _outStrings, _str, _delims );
        }
        //////////////////////////////////////////////////////////////////////////
        void wsplit( VectorWString * const _outStrings, const WString & _str, bool _trimDelims, const WString & _delims )
        {
            uint32_t numSplits = 0;
            WString::size_type start = 0;
            WString::size_type pos = 0;

            do
            {
                pos = _str.find_first_of( _delims, start );

                if( pos == WString::npos )
                {
                    _outStrings->emplace_back( _str.substr( start ) );

                    break;
                }
                else
                {
                    _outStrings->emplace_back( _str.substr( start, pos - start ) );

                    start = pos + 1;
                }

                if( _trimDelims == true )
                {
                    start = _str.find_first_not_of( _delims, start );
                }

                ++numSplits;

            } while( pos != WString::npos );
        }
        //////////////////////////////////////////////////////////////////////////
        void join( const String & _delim, const VectorString & _stringArray, String * const _outString )
        {
            if( _stringArray.empty() == true )
            {
                _outString->clear();

                return;
            }

            VectorString::const_iterator it = _stringArray.begin();

            *_outString = *it;

            ++it;

            for( VectorString::const_iterator it_end = _stringArray.end();
                it != it_end;
                ++it )
            {
                *_outString += _delim;
                *_outString += (*it);
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void trim( String * const _str, bool _left, bool _right )
        {
            const Char * delims = " \t\r";

            if( _right == true )
            {
                _str->erase( _str->find_last_not_of( delims ) + 1 );
            }

            if( _left == true )
            {
                _str->erase( 0, _str->find_first_not_of( delims ) );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        bool toupper( const Char * _str, Char * _upper, size_t _capacity )
        {
            size_t len = MENGINE_STRLEN( _str );

            if( len >= _capacity )
            {
                return false;
            }

            for( size_t index = 0, index_end = len;
                index != index_end;
                ++index )
            {
                Char ch = _str[index];

                _upper[index] = (Char)MENGINE_TOUPPER( ch );
            }

            _upper[len] = '\0';

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void toupper( const String & _in, String * const _upper )
        {
            Algorithm::transform( _in.begin(), _in.end(), _upper->begin(),
                []( Char c )
            {
                return (Char)MENGINE_TOUPPER( c );
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        bool tolower( const Char * _str, Char * _lower, size_t _capacity )
        {
            size_t len = MENGINE_STRLEN( _str );

            if( len >= _capacity )
            {
                return false;
            }

            for( size_t index = 0, index_end = len;
                index != index_end;
                ++index )
            {
                Char ch = _str[index];

                _lower[index] = (Char)MENGINE_TOLOWER( ch );
            }

            _lower[len] = '\0';

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void tolower( const String & _str, String * const _lower )
        {
            Algorithm::transform( _str.begin(), _str.end(), _lower->begin(),
                []( Char c )
            {
                return (Char)MENGINE_TOLOWER( c );
            } );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}