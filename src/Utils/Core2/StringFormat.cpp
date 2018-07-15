#include "StringFormat.h"

#include "Logger/Logger.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#   pragma warning(disable:4800)  
#   endif

#include "boost/format.hpp"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop) 
#   endif

#include "stdex/stl_allocator.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        typedef boost::basic_format<Char, std::char_traits<Char>, stdex::stl_allocator<Char> > StringFormat;
        //////////////////////////////////////////////////////////////////////////
        uint32_t getStringFormatExpectedArgs( const String & _format )
        {
            const Char * str_format = _format.c_str();

            try
            {
                StringFormat fmt( str_format );

                int expected_args = fmt.expected_args();

                return (uint32_t)expected_args;
            }
            catch( const boost::io::format_error & _ex )
            {
                MENGINE_THROW_EXCEPTION( "%s", _ex.what() );
            }

            return 0U;
        }
        //////////////////////////////////////////////////////////////////////////
        String getStringFormat( const String & _format, const TVectorString & _arguments )
        {
            const Char * str_format = _format.c_str();

            try
            {
                StringFormat fmt( str_format );

                for( const String & arg : _arguments )
                {
                    fmt % arg;
                }

                String fmt_str = fmt.str();

                return fmt_str;
            }
            catch( const boost::io::format_error & _ex )
            {
                MENGINE_THROW_EXCEPTION( "%s", _ex.what() );
            }

            return "";
        }
    }
}