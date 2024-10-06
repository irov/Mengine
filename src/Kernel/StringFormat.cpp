#include "StringFormat.h"

#include "Kernel/Logger.h"
#include "Kernel/Regex.h"
#include "Kernel/StringRegex.h"

#include "Config/StdIO.h"

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
                                return MENGINE_UNKNOWN_SIZE;
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
        bool fillStringFormat( const Char * _format, size_t _size, const VectorString & _arguments, String * const _out )
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
        bool fillStringTag( const String & _text, const Char * _tag, const LambdaStringTag & _lambda, String * const _out, bool * const _matched )
        {
            _out->clear();

            Char regex_format[64 + 1] = {'\0'};
            MENGINE_SNPRINTF( regex_format, 64, "<%s>(\\w+)</%s>"
                , _tag
                , _tag
            );

            String text_value = _text;
            String tag_value;
            String match_value;
            bool match_found = false;

            try
            {
                StringRegex r( regex_format );
                StringMatchResults match;
                while( Regex::regex_search( text_value, match, r ) == true )
                {
                    match_value.assign( match[1].first, match[1].second );

                    const Regex::sub_match<String::const_iterator> & match_prefix = match.prefix();
                    const Regex::sub_match<String::const_iterator> & match_suffix = match.suffix();

                    if( _lambda( match_value, &tag_value ) == false )
                    {
                        return false;
                    }

                    _out->append( match_prefix.first, match_prefix.second );
                    _out->append( tag_value );

                    text_value.assign( match_suffix.first, match_suffix.second );

                    match_found = true;
                }

                _out->append( text_value );
            }
            catch( const std::exception & _ex )
            {
                LOGGER_ERROR( "fill string '%s' tag '%s' catch exception '%s'"
                    , _text.c_str(), _tag, _ex.what()
                );
            }

            *_matched = match_found;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}