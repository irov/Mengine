#include "JSONHelper.h"

#include "Kernel/FileStreamHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/FileGroupHelper.h"

#include "Config/StdString.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            struct json_load_data
            {
                const uint8_t * buffer;
                size_t carriage;
                size_t capacity;

#if defined(MENGINE_DOCUMENT_ENABLE)
                DocumentInterfacePtr doc;
#endif
            };
            //////////////////////////////////////////////////////////////////////////
            static void __jpp_error( int32_t _line, int32_t _column, int32_t _position, const char * _source, const char * _text, void * _ud )
            {
                MENGINE_UNUSED( _ud );

                json_load_data * jd = static_cast<json_load_data *>(_ud);

                MENGINE_UNUSED( jd );

                LOGGER_ASSERTION( "jpp error: %s\nline: %d\n column: %d\nposition: %d\nsource: %s\ndoc: %s"
                    , _text
                    , _line
                    , _column
                    , _position
                    , _source
                    , MENGINE_DOCUMENT_STR( jd->doc )
                );
            }
            //////////////////////////////////////////////////////////////////////////
            static size_t __jpp_load_callback( void * _buffer, size_t _buflen, void * _ud )
            {
                json_load_data * jd = static_cast<json_load_data *>(_ud);

                if( _buflen > jd->capacity - jd->carriage )
                {
                    _buflen = jd->capacity - jd->carriage;
                }

                if( _buflen <= 0 )
                {
                    return 0;
                }

                const uint8_t * jd_buffer = jd->buffer + jd->carriage;
                stdex::memorycopy( _buffer, 0, jd_buffer, _buflen );
                jd->carriage += _buflen;

                return _buflen;
            }
            //////////////////////////////////////////////////////////////////////////
            static int __jpp_dump_stream_callback( const char * _buffer, jpp::jpp_size_t _size, void * _ud )
            {
                OutputStreamInterface * stream = static_cast<OutputStreamInterface *>(_ud);

                stream->write( _buffer, _size );

                return 0;
            }
            //////////////////////////////////////////////////////////////////////////
            static int __jpp_dump_string_callback( const char * _buffer, jpp::jpp_size_t _size, void * _ud )
            {
                String * string = static_cast<String *>(_ud);

                string->append( _buffer, _size );

                return 0;
            }
            //////////////////////////////////////////////////////////////////////////
            static int __jpp_dump_data_callback( const char * _buffer, jpp::jpp_size_t _size, void * _ud )
            {
                Data * data = static_cast<Data *>(_ud);

                data->insert( data->end(), _buffer, _buffer + _size );

                return 0;
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        jpp::object loadJSONFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentInterfacePtr & _doc )
        {
            InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open file '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath ).c_str()
            );

            jpp::object j = Helper::loadJSONStream( stream, _doc );

            return j;
        }
        //////////////////////////////////////////////////////////////////////////
        jpp::object loadJSONStream( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _stream, "invalid input stream" );

            MemoryInterfacePtr memory = Helper::createMemoryStream( _stream, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory stream" );

            jpp::object j = Helper::loadJSONMemory( memory, _doc );

            return j;
        }
        //////////////////////////////////////////////////////////////////////////
        jpp::object loadJSONMemory( const MemoryInterfacePtr & _memory, const DocumentInterfacePtr & _doc )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _memory, "invalid memory" );

            const void * memory_buffer = _memory->getBuffer();
            size_t memory_size = _memory->getSize();

            jpp::object j = Helper::loadJSONBuffer( memory_buffer, memory_size, _doc );

            return j;
        }
        //////////////////////////////////////////////////////////////////////////
        jpp::object loadJSONString( const String & _value, const DocumentInterfacePtr & _doc )
        {
            const String::value_type * value_buffer = _value.c_str();
            String::size_type value_size = _value.size();

            jpp::object j = Helper::loadJSONBuffer( value_buffer, value_size, _doc );

            return j;
        }
        //////////////////////////////////////////////////////////////////////////
        jpp::object loadJSONBuffer( const void * _buffer, size_t _size, const DocumentInterfacePtr & _doc )
        {
            MENGINE_UNUSED( _doc );

            Detail::json_load_data jd;
            jd.buffer = static_cast<const uint8_t *>(_buffer);
            jd.carriage = 0;
            jd.capacity = _size;

#if defined(MENGINE_DOCUMENT_ENABLE)
            jd.doc = _doc;
#endif

            jpp::object json = jpp::load( &Detail::__jpp_load_callback, jpp::JPP_LOAD_MODE_DISABLE_EOF_CHECK, &Detail::__jpp_error, &jd );

            if( json == jpp::detail::invalid )
            {
                return jpp::make_invalid();
            }

            return json;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONContent( const jpp::object & _j, const ContentInterfacePtr & _content, bool _withTemp, const DocumentInterfacePtr & _doc )
        {
            const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();
            const FilePath & filePath = _content->getFilePath();

            if( Helper::writeJSONFile( _j, fileGroup, filePath, _withTemp, _doc ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONFile( const jpp::object & _j, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _withTemp, const DocumentInterfacePtr & _doc )
        {
            OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( _fileGroup, _filePath, _withTemp, _doc );

            if( stream == nullptr )
            {
                return false;
            }

            bool successful = Helper::writeJSONStream( _j, stream );

            Helper::closeOutputStreamFile( _fileGroup, stream );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONFileCompact( const jpp::object & _j, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _withTemp, const DocumentInterfacePtr & _doc )
        {
            OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( _fileGroup, _filePath, _withTemp, _doc );

            if( stream == nullptr )
            {
                return false;
            }

            bool successful = Helper::writeJSONStreamCompact( _j, stream );

            Helper::closeOutputStreamFile( _fileGroup, stream );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONStream( const jpp::object & _j, const OutputStreamInterfacePtr & _stream )
        {
            if( jpp::dump( _j, &Detail::__jpp_dump_stream_callback, _stream.get() ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONStreamCompact( const jpp::object & _j, const OutputStreamInterfacePtr & _stream )
        {
            if( jpp::dump_compact( _j, &Detail::__jpp_dump_stream_callback, _stream.get() ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONString( const jpp::object & _j, String * const _string )
        {
            if( jpp::dump( _j, &Detail::__jpp_dump_string_callback, _string ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONStringCompact( const jpp::object & _j, String * const _string )
        {
            if( jpp::dump_compact( _j, &Detail::__jpp_dump_string_callback, _string ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONData( const jpp::object & _j, Data * const _data )
        {
            if( jpp::dump( _j, &Detail::__jpp_dump_data_callback, _data ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONDataCompact( const jpp::object & _j, Data * const _data )
        {
            if( jpp::dump_compact( _j, &Detail::__jpp_dump_data_callback, _data ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
