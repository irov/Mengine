#include "StreamHelper.h"

#include "Interface/ArchiveServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/CRC32.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DebugFileHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void readStreamData( const InputStreamInterfacePtr & _stream, Data * const _data )
        {
            size_t stream_size = _stream->size();

            _data->resize( stream_size );

            uint8_t * data_buffer = _data->data();

            _stream->read( data_buffer, stream_size );
        }
        //////////////////////////////////////////////////////////////////////////
        void writeStreamData( const OutputStreamInterfacePtr & _stream, const Data & _data )
        {
            const uint8_t * data_buffer = _data.data();
            size_t data_size = _data.size();

            _stream->write( data_buffer, data_size );
        }
        //////////////////////////////////////////////////////////////////////////
        bool readStreamMagicHeader( const InputStreamInterfacePtr & _stream, magic_number_type _magic, magic_version_type _version )
        {
            magic_number_type magic_number;
            if( _stream->read( &magic_number, sizeof( magic_number ) ) != sizeof( magic_number ) )
            {
                LOGGER_ERROR( "invalid stream '%s' format stream magic head"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return false;
            }

            if( magic_number != _magic )
            {
                LOGGER_ERROR( "invalid stream '%s' magic number '%u' need '%u'"
                    , Helper::getDebugFullPath( _stream )
                    , magic_number
                    , _magic
                );

                return false;
            }

            magic_version_type magic_version;
            if( _stream->read( &magic_version, sizeof( magic_version ) ) != sizeof( magic_version ) )
            {
                LOGGER_ERROR( "invalid stream '%s' format stream magic head"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return false;
            }

            if( magic_version != _version )
            {
                LOGGER_ERROR( "invalid stream '%s' magic version '%u' need '%u'"
                    , Helper::getDebugFullPath( _stream )
                    , magic_version
                    , _version
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeStreamMagicHeader( const OutputStreamInterfacePtr & _stream, magic_number_type _magic, magic_version_type _version )
        {
            if( _stream->write( &_magic, sizeof( _magic ) ) == false )
            {
                LOGGER_ERROR( "invalid stream '%s' write 'magic header'"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return false;
            }

            if( _stream->write( &_version, sizeof( _version ) ) == false )
            {
                LOGGER_ERROR( "invalid stream '%s' write 'magic version'"
                    , Helper::getDebugFullPath( _stream )
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr readStreamArchiveBuffer( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, const DocumentInterfacePtr & _doc )
        {
            uint32_t crc32;
            if( _stream->read( &crc32, sizeof( crc32 ) ) != sizeof( crc32 ) )
            {
                LOGGER_ERROR( "invalid file '%s' format stream archive buffer"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return nullptr;
            }

            uint32_t load_binary_size;
            if( _stream->read( &load_binary_size, sizeof( load_binary_size ) ) != sizeof( load_binary_size ) )
            {
                LOGGER_ERROR( "invalid stream '%s' format stream archive buffer"
                    , Helper::getDebugFullPath( _stream )
                );

                return nullptr;
            }

            uint32_t load_compress_size;
            if( _stream->read( &load_compress_size, sizeof( load_compress_size ) ) != sizeof( load_compress_size ) )
            {
                LOGGER_ERROR( "invalid stream '%s' format stream archive buffer"
                    , Helper::getDebugFullPath( _stream )
                );

                return nullptr;
            }

            size_t binary_size = load_binary_size;
            size_t compress_size = load_compress_size;

            MemoryInterfacePtr compress_buffer = Helper::createMemoryCacheBuffer( compress_size, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( compress_buffer, "invalid get memory '%zu' (compress)"
                , compress_size
            );

            void * compress_memory = compress_buffer->getBuffer();

            size_t read_data = _stream->read( compress_memory, compress_size );

            if( read_data != (size_t)compress_size )
            {
                LOGGER_ERROR( "invalid stream '%s' read data '%zu' need '%zu'"
                    , Helper::getDebugFullPath( _stream )
                    , read_data
                    , compress_size                    
                );

                return nullptr;
            }

            if( crc32 != 0 )
            {
                uint32_t check_crc32 = Helper::makeCRC32( compress_memory, compress_size );

                if( check_crc32 != crc32 )
                {
                    LOGGER_ERROR( "invalid stream '%s' crc32 [%u] need [%u]"
                        , Helper::getDebugFullPath( _stream )
                        , check_crc32
                        , crc32
                    );

                    return nullptr;
                }
            }

            MemoryBufferInterfacePtr binaryBuffer = MEMORY_SERVICE()
                ->createMemoryCacheBuffer( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( binaryBuffer, "invalid create memory cache" );

            void * binaryMemory = binaryBuffer->newBuffer( binary_size );

            MENGINE_ASSERTION_MEMORY_PANIC( binaryMemory, "invalid get memory '%zu' (binary)"
                , binary_size
            );

            size_t uncompressSize = 0;
            if( _archivator->decompress( binaryMemory, binary_size, compress_memory, compress_size, &uncompressSize ) == false )
            {
                LOGGER_ERROR( "invalid stream '%s' decompress doc '%s'"
                    , Helper::getDebugFullPath( _stream )
                    , MENGINE_DOCUMENT_STR( _doc )
                );

                return nullptr;
            }

            if( uncompressSize != binary_size )
            {
                LOGGER_ERROR( "invalid stream '%s' decompress size '%zu' need '%zu'"
                    , Helper::getDebugFullPath( _stream )
                    , uncompressSize
                    , binary_size
                );

                return nullptr;
            }

            return binaryBuffer;
        }
        //////////////////////////////////////////////////////////////////////////
        bool readStreamArchiveInplace( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, void * const _data, size_t _capacity, size_t * const _size, const DocumentInterfacePtr & _doc )
        {
            uint32_t crc32;
            if( _stream->read( &crc32, sizeof( crc32 ) ) != sizeof( crc32 ) )
            {
                LOGGER_ERROR( "invalid stream '%s' format stream archive inplace"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return false;
            }

            uint32_t load_binary_size;
            if( _stream->read( &load_binary_size, sizeof( load_binary_size ) ) != sizeof( load_binary_size ) )
            {
                LOGGER_ERROR( "invalid stream '%s' format stream archive inplace"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return false;
            }

            uint32_t load_compress_size;
            if( _stream->read( &load_compress_size, sizeof( load_compress_size ) ) != sizeof( load_compress_size ) )
            {
                LOGGER_ERROR( "invalid stream '%s' format stream archive inplace"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return false;
            }

            uint32_t binary_size = load_binary_size;
            uint32_t compress_size = load_compress_size;

            if( binary_size > _capacity )
            {
                LOGGER_ERROR( "invalid stream '%s' buffer size '%zu' need '%u'"
                    , Helper::getDebugFullPath( _stream )
                    , _capacity
                    , binary_size
                );

                return false;
            }

            MemoryInterfacePtr compress_buffer = Helper::createMemoryCacheStreamSize( _stream, compress_size, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( compress_buffer, "invalid get memory '%u' (compress)"
                , compress_size
            );

            void * compress_memory = compress_buffer->getBuffer();

            if( crc32 != 0 )
            {
                uint32_t check_crc32 = Helper::makeCRC32( compress_memory, compress_size );

                if( check_crc32 != crc32 )
                {
                    LOGGER_ERROR( "invalid stream '%s' crc32 '%u' need '%u'"
                        , Helper::getDebugFullPath( _stream )
                        , check_crc32
                        , crc32
                    );

                    return false;
                }
            }

            size_t uncompressSize = 0;
            if( _archivator->decompress( _data, _capacity, compress_memory, compress_size, &uncompressSize ) == false )
            {
                LOGGER_ERROR( "invalid stream '%s' decompress"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return false;
            }

            if( uncompressSize > _capacity )
            {
                LOGGER_ERROR( "invalid stream '%s' decompress size '%zu' need '%zu'"
                    , Helper::getDebugFullPath( _stream )
                    , uncompressSize
                    , _capacity
                );

                return false;
            }

            if( _size != nullptr )
            {
                *_size = uncompressSize;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool readStreamArchiveBufferSize( const InputStreamInterfacePtr & _stream, size_t * const _size )
        {
            size_t pos = _stream->tell();

            uint32_t crc32;
            if( _stream->read( &crc32, sizeof( crc32 ) ) != sizeof( crc32 ) )
            {
                LOGGER_ERROR( "invalid stream '%s' read crc32"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return false;
            }

            uint32_t load_binary_size;
            if( _stream->read( &load_binary_size, sizeof( load_binary_size ) ) != sizeof( load_binary_size ) )
            {
                LOGGER_ERROR( "invalid stream '%s' read binary size"
                    , Helper::getDebugFullPath( _stream ) 
                );


                return false;
            }

            if( _stream->seek( pos ) == false )
            {
                LOGGER_ERROR( "invalid stream '%s' rewind"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return false;
            }

            if( _size != nullptr )
            {
                *_size = (size_t)load_binary_size;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeStreamArchiveBuffer( const OutputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, bool _crc32, const void * _data, size_t _size, EArchivatorCompress _compress )
        {
            MemoryInputInterfacePtr compress_memory = ARCHIVE_SERVICE()
                ->compressBuffer( _archivator, _data, _size, _compress );

            MENGINE_ASSERTION_MEMORY_PANIC( compress_memory, "invalid compress" );

            const void * compressBuffer = compress_memory->getBuffer();
            size_t compressSize = compress_memory->getSize();

            MENGINE_ASSERTION_MEMORY_PANIC( compressBuffer, "invalid get memory" );

            uint32_t value_crc32 = _crc32 == true ? Helper::makeCRC32( compressBuffer, compressSize ) : 0;

            if( _stream->write( &value_crc32, sizeof( value_crc32 ) ) == false )
            {
                LOGGER_ERROR( "invalid stream '%s' write 'crc32'"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return false;
            }

            uint32_t write_uncompressSize = (uint32_t)_size;
            if( _stream->write( &write_uncompressSize, sizeof( write_uncompressSize ) ) == false )
            {
                LOGGER_ERROR( "invalid stream '%s' write 'uncompress size'"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return false;
            }

            uint32_t write_compressSize = (uint32_t)compressSize;
            if( _stream->write( &write_compressSize, sizeof( write_compressSize ) ) == false )
            {
                LOGGER_ERROR( "invalid stream '%s' write 'compress size'"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return false;
            }

            if( _stream->write( compressBuffer, compressSize ) == false )
            {
                LOGGER_ERROR( "invalid stream '%s' write buffer '%zu'"
                    , Helper::getDebugFullPath( _stream )
                    , compressSize
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr readStreamArchiveMagic( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, const DocumentInterfacePtr & _doc )
        {
            if( Helper::readStreamMagicHeader( _stream, _magic, _version ) == false )
            {
                return nullptr;
            }

            MemoryInterfacePtr binaryBuffer = Helper::readStreamArchiveBuffer( _stream, _archivator, _doc );

            return binaryBuffer;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeStreamArchiveMagic( const OutputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, bool _crc32, const void * _data, size_t _size, EArchivatorCompress _compress )
        {
            if( Helper::writeStreamMagicHeader( _stream, _magic, _version ) == false )
            {
                return false;
            }

            if( Helper::writeStreamArchiveBuffer( _stream, _archivator, _crc32, _data, _size, _compress ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr readStreamArchiveMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, const DocumentInterfacePtr & _doc )
        {
            uint32_t crc32;
            if( _stream->read( &crc32, sizeof( crc32 ) ) != sizeof( crc32 ) )
            {
                LOGGER_ERROR( "invalid stream '%s' format stream archive memory"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return nullptr;
            }

            uint32_t load_binary_size;
            if( _stream->read( &load_binary_size, sizeof( load_binary_size ) ) != sizeof( load_binary_size ) )
            {
                LOGGER_ERROR( "invalid stream '%s' format stream archive memory"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return nullptr;
            }

            uint32_t load_compress_size;
            if( _stream->read( &load_compress_size, sizeof( load_compress_size ) ) != sizeof( load_compress_size ) )
            {
                LOGGER_ERROR( "invalid stream '%s' format stream archive memory"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return nullptr;
            }

            size_t binary_size = load_binary_size;
            size_t compress_size = load_compress_size;

            MemoryInterfacePtr compress_memory = Helper::createMemoryCacheBuffer( compress_size, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( compress_memory, "invalid get memory '%zu' (compress)"
                , compress_size
            );

            void * compress_buffer = compress_memory->getBuffer();

            size_t read_data = _stream->read( compress_buffer, compress_size );

            if( read_data != (size_t)compress_size )
            {
                LOGGER_ERROR( "invalid stream '%s' read data '%zu' need '%zu'"
                    , Helper::getDebugFullPath( _stream )
                    , read_data
                    , compress_size
                );

                return nullptr;
            }

            if( crc32 != 0 )
            {
                uint32_t check_crc32 = Helper::makeCRC32( compress_buffer, compress_size );

                if( check_crc32 != crc32 )
                {
                    LOGGER_ERROR( "invalid stream '%s' crc32 [%u] need [%u]"
                        , Helper::getDebugFullPath( _stream )
                        , check_crc32
                        , crc32
                    );

                    return nullptr;
                }
            }

            MemoryBufferInterfacePtr binary_memory = MEMORY_SERVICE()
                ->createMemoryBuffer( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( binary_memory, "invalid get memory '%zu' (binary)"
                , binary_size
            );

            void * binary_buffer = binary_memory->newBuffer( binary_size );

            size_t uncompressSize = 0;
            if( _archivator->decompress( binary_buffer, binary_size, compress_buffer, compress_size, &uncompressSize ) == false )
            {
                LOGGER_ERROR( "invalid stream '%s' decompress"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return nullptr;
            }

            if( uncompressSize != binary_size )
            {
                LOGGER_ERROR( "invalid stream '%s' decompress size '%zu' need '%zu'"
                    , Helper::getDebugFullPath( _stream )
                    , uncompressSize
                    , binary_size
                );

                return nullptr;
            }

            return binary_memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr readStreamArchiveMagicMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, const DocumentInterfacePtr & _doc )
        {
            if( Helper::readStreamMagicHeader( _stream, _magic, _version ) == false )
            {
                return nullptr;
            }

            MemoryInterfacePtr memory = Helper::readStreamArchiveMemory( _stream, _archivator, _doc );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr readStreamCacheArchiveMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, const DocumentInterfacePtr & _doc )
        {
            uint32_t crc32;
            if( _stream->read( &crc32, sizeof( crc32 ) ) != sizeof( crc32 ) )
            {
                LOGGER_ERROR( "invalid stream '%s' format stream cache archive memory"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return nullptr;
            }

            uint32_t load_binary_size;
            if( _stream->read( &load_binary_size, sizeof( load_binary_size ) ) != sizeof( load_binary_size ) )
            {
                LOGGER_ERROR( "invalid stream '%s' format stream cache archive memory"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return nullptr;
            }

            uint32_t load_compress_size;
            if( _stream->read( &load_compress_size, sizeof( load_compress_size ) ) != sizeof( load_compress_size ) )
            {
                LOGGER_ERROR( "invalid stream '%s' format stream cache archive memory"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return nullptr;
            }

            size_t binary_size = load_binary_size;
            size_t compress_size = load_compress_size;

            MemoryInterfacePtr compress_memory = Helper::createMemoryCacheBuffer( compress_size, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( compress_memory, "invalid get memory '%zu' (compress)"
                , compress_size
            );

            void * compress_buffer = compress_memory->getBuffer();

            size_t read_data = _stream->read( compress_buffer, compress_size );

            if( read_data != compress_size )
            {
                LOGGER_ERROR( "invalid stream '%s' read data '%zu' need '%zu'"
                    , Helper::getDebugFullPath( _stream )
                    , read_data
                    , compress_size
                );

                return nullptr;
            }

            if( crc32 != 0 )
            {
                uint32_t check_crc32 = Helper::makeCRC32( compress_buffer, compress_size );

                if( check_crc32 != crc32 )
                {
                    LOGGER_ERROR( "invalid stream '%s' crc32 '%u' need '%u'"
                        , Helper::getDebugFullPath( _stream )
                        , check_crc32
                        , crc32
                    );

                    return nullptr;
                }
            }

            MemoryBufferInterfacePtr binary_memory = MEMORY_SERVICE()
                ->createMemoryCacheBuffer( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( binary_memory, "invalid get memory '%zu' (binary)"
                , binary_size
            );

            void * binary_buffer = binary_memory->newBuffer( binary_size );

            size_t uncompressSize = 0;
            if( _archivator->decompress( binary_buffer, binary_size, compress_buffer, compress_size, &uncompressSize ) == false )
            {
                LOGGER_ERROR( "invalid stream '%s' decompress"
                    , Helper::getDebugFullPath( _stream ) 
                );

                return nullptr;
            }

            if( uncompressSize != binary_size )
            {
                LOGGER_ERROR( "invalid stream '%s' decompress size '%zu' need '%zu'"
                    , Helper::getDebugFullPath( _stream )
                    , uncompressSize
                    , binary_size
                );

                return nullptr;
            }

            return binary_memory;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
