#include "JSONHelper.h"

#include "Kernel/FileStreamHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            struct my_json_load_data_t
            {
                const uint8_t * buffer;
                size_t carriage;
                size_t capacity;

#if defined(MENGINE_DOCUMENT_ENABLE)
                DocumentInterfacePtr doc;
#endif
            };
            //////////////////////////////////////////////////////////////////////////
            static void my_jpp_error( int32_t _line, int32_t _column, int32_t _position, const char * _source, const char * _text, void * _ud )
            {
                MENGINE_UNUSED( _ud );

                my_json_load_data_t * jd = static_cast<my_json_load_data_t *>(_ud);

                MENGINE_UNUSED( jd );

                LOGGER_ERROR( "jpp error: %s\nline: %d\n column: %d\nposition: %d\nsource: %s\ndoc: %s"
                    , _text
                    , _line
                    , _column
                    , _position
                    , _source
                    , MENGINE_DOCUMENT_STR( jd->doc )
                );
            }
            //////////////////////////////////////////////////////////////////////////
            static size_t my_jpp_load_callback( void * _buffer, size_t _buflen, void * _ud )
            {
                my_json_load_data_t * jd = static_cast<my_json_load_data_t *>(_ud);

                if( _buflen > jd->capacity - jd->carriage )
                {
                    _buflen = jd->capacity - jd->carriage;
                }

                if( _buflen <= 0 )
                {
                    return 0;
                }

                const uint8_t * jd_buffer = jd->buffer + jd->carriage;
                MENGINE_MEMCPY( _buffer, jd_buffer, _buflen );
                jd->carriage += _buflen;

                return _buflen;
            }
            //////////////////////////////////////////////////////////////////////////
            static int my_jpp_dump_stream_callback( const char * _buffer, jpp::jpp_size_t _size, void * _ud )
            {
                OutputStreamInterface * stream = static_cast<OutputStreamInterface *>(_ud);

                stream->write( _buffer, _size );

                return 0;
            }
            //////////////////////////////////////////////////////////////////////////
            static int my_jpp_dump_string_callback( const char * _buffer, jpp::jpp_size_t _size, void * _ud )
            {
                String * string = static_cast<String *>(_ud);

                string->append( _buffer, _size );

                return 0;
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        jpp::object loadJSON( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentInterfacePtr & _doc )
        {
            InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream );

            jpp::object j = Helper::loadJSONStream( stream, _doc );

            return j;
        }
        //////////////////////////////////////////////////////////////////////////
        jpp::object loadJSONStream( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _stream );

            MemoryInterfacePtr memory = Helper::createMemoryStream( _stream, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory );

            jpp::object j = Helper::loadJSONStreamFromMemory( memory, _doc );

            return j;
        }
        //////////////////////////////////////////////////////////////////////////
        jpp::object loadJSONStreamFromMemory( const MemoryInterfacePtr & _memory, const DocumentInterfacePtr & _doc )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _memory );

            const void * memory_buffer = _memory->getBuffer();
            size_t memory_size = _memory->getSize();

            jpp::object j = Helper::loadJSONStreamFromBuffer( memory_buffer, memory_size, _doc );

            return j;
        }
        //////////////////////////////////////////////////////////////////////////
        jpp::object loadJSONStreamFromString( const String & _value, const DocumentInterfacePtr & _doc )
        {
            const String::value_type * value_buffer = _value.c_str();
            String::size_type value_size = _value.size();

            jpp::object j = Helper::loadJSONStreamFromBuffer( value_buffer, value_size, _doc );

            return j;
        }
        //////////////////////////////////////////////////////////////////////////
        jpp::object loadJSONStreamFromBuffer( const void * _buffer, size_t _size, const DocumentInterfacePtr & _doc )
        {
            MENGINE_UNUSED( _doc );

            Detail::my_json_load_data_t jd;
            jd.buffer = static_cast<const uint8_t *>(_buffer);
            jd.carriage = 0;
            jd.capacity = _size;

#if defined(MENGINE_DOCUMENT_ENABLE)
            jd.doc = _doc;
#endif

            jpp::object json = jpp::load( &Detail::my_jpp_load_callback, jpp::JPP_LOAD_MODE_DISABLE_EOF_CHECK, &Detail::my_jpp_error, &jd );

            if( json == jpp::detail::invalid )
            {
                return jpp::make_invalid();
            }

            return json;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONFile( const jpp::object & _j, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _withTemp, const DocumentInterfacePtr & _doc )
        {
            OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( _fileGroup, _filePath, _withTemp, _doc );

            if( stream == nullptr )
            {
                return false;
            }

            if( Helper::writeJSONStream( _j, stream ) == false )
            {
                return false;
            }

            if( Helper::closeOutputStreamFile( _fileGroup, stream ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONFileCompact( const jpp::object & _j, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _withTemp, const DocumentInterfacePtr & _doc )
        {
            OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( _fileGroup, _filePath, _withTemp, _doc );

            if( stream == nullptr )
            {
                return false;
            }

            if( Helper::writeJSONStreamCompact( _j, stream ) == false )
            {
                return false;
            }

            if( Helper::closeOutputStreamFile( _fileGroup, stream ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONStream( const jpp::object & _j, const OutputStreamInterfacePtr & _stream )
        {
            if( jpp::dump( _j, &Detail::my_jpp_dump_stream_callback, _stream.get() ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONStreamCompact( const jpp::object & _j, const OutputStreamInterfacePtr & _stream )
        {
            if( jpp::dump_compact( _j, &Detail::my_jpp_dump_stream_callback, _stream.get() ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONString( const jpp::object & _j, String * const _string )
        {
            if( jpp::dump( _j, &Detail::my_jpp_dump_string_callback, _string ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSONStringCompact( const jpp::object & _j, String * const _string )
        {
            if( jpp::dump_compact( _j, &Detail::my_jpp_dump_string_callback, _string ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
