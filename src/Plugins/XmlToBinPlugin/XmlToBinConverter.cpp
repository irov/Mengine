#include "XmlToBinConverter.h"

#include "Interface/LoaderServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/Data.h"
#include "Kernel/ParamsHelper.h"
#include "Kernel/ContentHelper.h"

#include "Config/StdString.h"
#include "Config/StdIntTypes.h"

#include "Metacode/Metacode.h"

#include "metabuf/Metadata.hpp"

#include "xml2metabuf/Xml2Metabuf.hpp"
#include "xml2metabuf/Xml2Metacode.hpp"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool xml_write_wstring( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
        {
            MENGINE_UNUSED( _user );

            uint32_t utf8_size = (uint32_t)StdString::strlen( _value );

            _metabuf->writeSize( utf8_size );
            _metabuf->writeCount( _value, utf8_size );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool xml_write_wchar_t( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
        {
            MENGINE_UNUSED( _user );

            uint32_t utf8_size = (uint32_t)StdString::strlen( _value );

            _metabuf->writeSize( utf8_size );
            _metabuf->writeCount( _value, utf8_size );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool xml_write_utf8( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
        {
            MENGINE_UNUSED( _user );

            size_t len = StdString::strlen( _value );
            const char * text_it = _value;
            const char * text_end = _value + len;

            uint32_t code;
            if( Helper::utf8NextCode( &text_it, text_end, &code ) == false )
            {
                return false;
            }

            _metabuf->write( code );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    XmlToBinConverter::XmlToBinConverter()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    XmlToBinConverter::~XmlToBinConverter()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool XmlToBinConverter::_initialize()
    {
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator, "not found 'lz4' archivator" );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void XmlToBinConverter::_finalize()
    {
        m_archivator = nullptr;
    }
    ////////////////////////////////////////////////////////////////////////////
    bool XmlToBinConverter::convert()
    {
        const ConverterOptions & options = this->getOptions();

        ContentInterfacePtr protocol_content = Helper::getParam( options.params, STRINGIZE_STRING_LOCAL( "protocolContent" ), ContentInterfacePtr::none() );
        ParamInteger useProtocolVersion = Helper::getParam( options.params, STRINGIZE_STRING_LOCAL( "useProtocolVersion" ), MENGINE_INT64_C( 0 ) );
        ParamInteger useProtocolCrc32 = Helper::getParam( options.params, STRINGIZE_STRING_LOCAL( "useProtocolCrc32" ), MENGINE_INT64_C( 0 ) );

        LOGGER_INFO( "convert", "xml to bin:\nxml - %s\nbin - %s\nversion - %d"
            , Helper::getContentFullPath( options.inputContent ).c_str()
            , Helper::getContentFullPath( options.outputContent ).c_str()
            , (int32_t)useProtocolVersion
        );

        InputStreamInterfacePtr protocol_stream = protocol_content->openInputStreamFile( false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( protocol_stream, "error open protocol '%s'"
            , Helper::getContentFullPath( protocol_content ).c_str()
        );

        size_t protocol_size = protocol_stream->size();

        MemoryBufferInterfacePtr memory_protocol = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory_protocol, "invalid create memory for protocol" );

        void * memory_protocol_buffer = memory_protocol->newBuffer( protocol_size );

        MENGINE_ASSERTION_MEMORY_PANIC( memory_protocol_buffer, "invalid new memory buffer '%zu'"
            , protocol_size
        );

        if( protocol_stream->read( memory_protocol_buffer, protocol_size ) != protocol_size )
        {
            LOGGER_ERROR( "error read protocol '%s' error invalid read size"
                , Helper::getContentFullPath( protocol_content ).c_str()
            );

            return false;
        }

        protocol_content->closeInputStreamFile( protocol_stream );
        protocol_stream = nullptr;

        Metabuf::XmlProtocol xml_protocol;

        if( xml_protocol.readProtocol( memory_protocol_buffer, protocol_size ) == false )
        {
            LOGGER_ERROR( "error read protocol '%s' error:\n%s"
                , Helper::getContentFullPath( protocol_content ).c_str()
                , xml_protocol.getError().c_str()
            );

            return false;
        }

        if( useProtocolVersion != xml_protocol.getVersion() )
        {
            LOGGER_ERROR( "protocol '%s' invalid version '%u' use '%" MENGINE_PRIu64 "'"
                , Helper::getContentFullPath( protocol_content ).c_str()
                , xml_protocol.getVersion()
                , useProtocolVersion
            );

            return false;
        }

        if( useProtocolCrc32 != xml_protocol.getCrc32() )
        {
            LOGGER_ERROR( "protocol '%s' invalid crc32 '%u' use '%" MENGINE_PRIu64 "'"
                , Helper::getContentFullPath( protocol_content ).c_str()
                , xml_protocol.getVersion()
                , useProtocolCrc32
            );

            return false;
        }

        memory_protocol = nullptr;

        InputStreamInterfacePtr xml_stream = options.inputContent->openInputStreamFile( false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( xml_stream, "error open xml '%s'"
            , Helper::getContentFullPath( options.inputContent ).c_str()
        );

        size_t xml_size = xml_stream->size();

        if( xml_size == 0 )
        {
            LOGGER_ERROR( "error open xml '%s' (file size == 0)"
                , Helper::getContentFullPath( options.inputContent ).c_str()
            );

            return false;
        }

        MemoryBufferInterfacePtr memory_xml = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory_xml, "invalid create memory for xml" );

        void * memory_xml_buffer = memory_xml->newBuffer( xml_size );

        MENGINE_ASSERTION_MEMORY_PANIC( memory_xml, "invalid new memory buffer '%zu'"
            , xml_size
        );

        if( xml_stream->read( memory_xml_buffer, xml_size ) != xml_size )
        {
            LOGGER_ERROR( "error read xml '%s' error invalid read size"
                , Helper::getContentFullPath( options.inputContent ).c_str()
            );

            return false;
        }

        options.inputContent->closeInputStreamFile( xml_stream );
        xml_stream = nullptr;

        const Metabuf::XmlMeta * xml_meta = xml_protocol.getMeta( "Data" );

        MENGINE_ASSERTION_MEMORY_PANIC( xml_meta, "invalid get meta 'Data' from protocol '%s'"
            , Helper::getContentFullPath( protocol_content ).c_str()
        );

        Metabuf::Xml2Metabuf xml_metabuf( &xml_protocol, xml_meta );

        xml_metabuf.initialize();

        xml_metabuf.addSerializator( "wstring", &Detail::xml_write_wstring, (void *)nullptr );
        xml_metabuf.addSerializator( "wchar_t", &Detail::xml_write_wchar_t, (void *)nullptr );
        xml_metabuf.addSerializator( "utf8", &Detail::xml_write_utf8, (void *)nullptr );

        MemoryBufferInterfacePtr memory_header = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory_header, "invalid create memory for bin" );

        void * memory_header_buffer = memory_header->newBuffer( Metacode::header_size );

        MENGINE_ASSERTION_MEMORY_PANIC( memory_header_buffer, "invalid new memory buffer '%zu'"
            , Metacode::header_size
        );

        uint32_t xml_meta_version = xml_meta->getVersion();

        size_t header_size;
        if( xml_metabuf.header( memory_header_buffer, Metacode::header_size, xml_meta_version, &header_size ) == false )
        {
            LOGGER_ERROR( "error header '%s' version '%u' error:\n%s"
                , Helper::getContentFullPath( options.inputContent ).c_str()
                , xml_meta_version
                , xml_metabuf.getError().c_str()
            );

            return false;
        }

        MemoryBufferInterfacePtr memory_bin = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory_bin, "invalid create memory for bin" );

        void * memory_bin_buffer = memory_bin->newBuffer( xml_size * 2 );

        MENGINE_ASSERTION_MEMORY_PANIC( memory_bin_buffer, "invalid new memory buffer '%zu'"
            , xml_size * 2
        );

        size_t bin_size;
        if( xml_metabuf.convert( memory_bin_buffer, xml_size * 2, memory_xml_buffer, xml_size, &bin_size ) == false )
        {
            LOGGER_ERROR( "error convert '%s' error:\n%s"
                , Helper::getContentFullPath( options.inputContent ).c_str()
                , xml_metabuf.getError().c_str()
            );

            return false;
        }

        MemoryInputInterfacePtr compress_memory = ARCHIVE_SERVICE()
            ->compressBuffer( m_archivator, memory_bin_buffer, bin_size, EAC_BEST );

        MENGINE_ASSERTION_MEMORY_PANIC( compress_memory, "error convert '%s' invalid compress buffer"
            , Helper::getContentFullPath( options.inputContent ).c_str()
        );

        OutputStreamInterfacePtr bin_stream = options.outputContent->openOutputStreamFile( true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( bin_stream, "error create bin '%s'"
            , Helper::getContentFullPath( options.outputContent ).c_str()
        );

        bin_stream->write( memory_header_buffer, Metacode::header_size );

        uint32_t write_bin_size = (uint32_t)bin_size;
        bin_stream->write( &write_bin_size, sizeof( write_bin_size ) );

        const void * compress_buffer = compress_memory->getBuffer();
        size_t compress_size = compress_memory->getSize();

        MENGINE_ASSERTION_MEMORY_PANIC( compress_buffer, "error create bin '%s' invalid get memory"
            , Helper::getContentFullPath( options.outputContent ).c_str()
        );

        uint32_t write_compress_size = (uint32_t)compress_size;
        bin_stream->write( &write_compress_size, sizeof( write_compress_size ) );
        bin_stream->write( compress_buffer, write_compress_size );

        bin_stream->flush();

        bool successful_close_stream = options.outputContent->closeOutputStreamFile( bin_stream );

        bin_stream = nullptr;

        if( successful_close_stream == false )
        {
            LOGGER_ERROR( "error create bin '%s' invalid close file for '%s'"
                , Helper::getContentFullPath( options.outputContent ).c_str()
                , Helper::getContentFullPath( options.inputContent ).c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
