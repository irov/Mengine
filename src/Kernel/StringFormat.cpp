#include "StringFormat.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        uint32_t getStringFormatExpectedArgs( const Char * _format, size_t _size )
        {
            uint32_t num_argumetns = 0;

            size_t format_size = _size;
            for( size_t index = 0; index < format_size; ++index )
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
        bool getStringFormat( String * const _out, const Char * _format, size_t _size, const VectorString & _arguments )
        {
            uint32_t num_argumetns = 0;

            size_t format_size = _size;
            for( size_t index = 0; index < format_size; ++index )
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
                                *_out += '%';
                            }break;
                        case 's':
                            {
                                if( num_argumetns >= _arguments.size() )
                                {
                                    return false;
                                }

                                *_out += _arguments[num_argumetns++];
                            }break;
                        default:
                            {
                                return false;
                            }break;
                        }
                    }break;
                default:
                    {
                        *_out += ch;
                    }break;
                }
            }

            if( num_argumetns != _arguments.size() )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}