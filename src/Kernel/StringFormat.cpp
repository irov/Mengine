#include "StringFormat.h"

#include "Kernel/Logger.h"

#include "stdex/stl_allocator.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        uint32_t getStringFormatExpectedArgs( const String & _format )
        {
            uint32_t num_argumetns = 0;

            String::size_type format_size = _format.size();
            for( String::size_type index = 0; index < format_size; ++index )
            {
                const Char ch = _format[index];

                switch( ch )
                {
                case '%':
                    {
                        ++index;

                        const Char ch1 = _format[index];
                        switch( ch1 )
                        {
                        case '%':
                            {
                            }break;
                        case 's':
                            {
                                num_argumetns++;
                            }break;
                        default:
                            {
                                return ~0U;
                            }break;
                        }
                    }break;
                default:
                    {
                    }break;
                }
            }

            return num_argumetns;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getStringFormat( String & _out, const String & _format, const VectorString & _arguments )
        {
            uint32_t num_argumetns = 0;

            String::size_type format_size = _format.size();
            for( String::size_type index = 0; index < format_size; ++index )
            {
                const Char ch = _format[index];

                switch( ch )
                {
                case '%':
                    {
                        ++index;

                        const Char ch1 = _format[index];
                        switch( ch1 )
                        {
                        case '%':
                            {
                                _out += '%';
                            }break;
                        case 's':
                            {
                                if( num_argumetns >= _arguments.size() )
                                {
                                    return false;
                                }

                                _out += _arguments[num_argumetns++];
                            }break;
                        default:
                            {
                                return false;
                            }break;
                        }
                    }break;
                default:
                    {
                        _out += ch;
                    }break;
                }
            }

            return true;
        }
    }
}