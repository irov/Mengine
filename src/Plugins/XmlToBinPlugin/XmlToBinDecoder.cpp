#include "XmlToBinDecoder.h"

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
#include "Kernel/Utf8Helper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/Data.h"

#include "Config/StdString.h"

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
            if( Helper::Utf8NextCode( &text_it, text_end, &code ) == false )
            {
                return false;
            }

            _metabuf->write( code );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    XmlToBinDecoder::XmlToBinDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    XmlToBinDecoder::~XmlToBinDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const InputStreamInterfacePtr & XmlToBinDecoder::getStream() const
    {
        return m_stream;
    }
    //////////////////////////////////////////////////////////////////////////
    void XmlToBinDecoder::setCodecDataInfo( const CodecDataInfo * _dataInfo )
    {
        MENGINE_UNUSED( _dataInfo );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const XmlCodecDataInfo * XmlToBinDecoder::getCodecDataInfo() const
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XmlToBinDecoder::initialize()
    {
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        m_archivator = archivator;

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup );

        m_fileGroupDev = fileGroup;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void XmlToBinDecoder::finalize()
    {
        m_archivator = nullptr;
        m_fileGroupDev = nullptr;

        m_stream = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XmlToBinDecoder::prepareData( const InputStreamInterfacePtr & _stream )
    {
        m_stream = _stream;

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////
    size_t XmlToBinDecoder::decode( const DecoderData * _decoderData )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _decoderData );
        MENGINE_ASSERTION_TYPE( _decoderData, const XmlDecoderData * );

        const XmlDecoderData * decoderData = static_cast<const XmlDecoderData *>(_decoderData);

        LOGGER_MESSAGE_RELEASE( "xml to bin:\nxml - %s\nbin - %s"
            , decoderData->pathXml.c_str()
            , decoderData->pathBin.c_str()
        );

        InputStreamInterfacePtr protocol_stream = Helper::openInputStreamFile( m_fileGroupDev, decoderData->pathProtocol, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( protocol_stream, "error open protocol '%s'"
            , decoderData->pathProtocol.c_str()
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
                , decoderData->pathProtocol.c_str()
            );

            return 0;
        }

        protocol_stream = nullptr;

        Metabuf::XmlProtocol xml_protocol;

        if( xml_protocol.readProtocol( memory_protocol_buffer, protocol_size ) == false )
        {
            LOGGER_ERROR( "error read protocol '%s' error:\n%s"
                , decoderData->pathProtocol.c_str()
                , xml_protocol.getError().c_str()
            );

            return 0;
        }

        if( decoderData->useProtocolVersion != xml_protocol.getVersion() )
        {
            LOGGER_ERROR( "protocol '%s' invalid version '%u' use '%u'"
                , decoderData->pathProtocol.c_str()
                , xml_protocol.getVersion()
                , decoderData->useProtocolVersion
            );

            return 0;
        }

        if( decoderData->useProtocolCrc32 != xml_protocol.getCrc32() )
        {
            LOGGER_ERROR( "protocol '%s' invalid crc32 '%u' use '%u'"
                , decoderData->pathProtocol.c_str()
                , xml_protocol.getVersion()
                , decoderData->useProtocolCrc32
            );

            return 0;
        }

        memory_protocol = nullptr;

        InputStreamInterfacePtr xml_stream = Helper::openInputStreamFile( m_fileGroupDev, decoderData->pathXml, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( xml_stream, "error open xml '%s'"
            , decoderData->pathXml.c_str()
        );

        size_t xml_size = xml_stream->size();

        if( xml_size == 0 )
        {
            LOGGER_ERROR( "error open xml '%s' (file size == 0)"
                , decoderData->pathXml.c_str()
            );

            return 0;
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
            return 0;
        }

        xml_stream = nullptr;

        const Metabuf::XmlMeta * xml_meta = xml_protocol.getMeta( "Data" );

        MENGINE_ASSERTION_MEMORY_PANIC( xml_meta );

        Metabuf::Xml2Metabuf xml_metabuf( &xml_protocol, xml_meta );

        LOGGER_INFO( "convert", "\nxml %s\nbin %s"
            , decoderData->pathXml.c_str()
            , decoderData->pathBin.c_str()
        );

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
                , decoderData->pathXml.c_str()
                , xml_meta_version
                , xml_metabuf.getError().c_str()
            );

            return 0;
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
                , decoderData->pathXml.c_str()
                , xml_metabuf.getError().c_str()
            );

            return 0;
        }

        MemoryInputInterfacePtr compress_memory = ARCHIVE_SERVICE()
            ->compressBuffer( m_archivator, memory_bin_buffer, bin_size, EAC_BEST );

        MENGINE_ASSERTION_MEMORY_PANIC( compress_memory, "error convert '%s' invalid compress buffer"
            , decoderData->pathXml.c_str()
        );

        OutputStreamInterfacePtr bin_stream = Helper::openOutputStreamFile( m_fileGroupDev, decoderData->pathBin, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( bin_stream, "error create bin '%s'"
            , decoderData->pathBin.c_str()
        );

        bin_stream->write( memory_header_buffer, Metacode::header_size );

        uint32_t write_bin_size = (uint32_t)bin_size;
        bin_stream->write( &write_bin_size, sizeof( write_bin_size ) );

        const void * compress_buffer = compress_memory->getBuffer();
        size_t compress_size = compress_memory->getSize();

        MENGINE_ASSERTION_MEMORY_PANIC( compress_buffer, "error create bin '%s' invalid get memory"
            , decoderData->pathBin.c_str()
        );

        uint32_t write_compress_size = (uint32_t)compress_size;
        bin_stream->write( &write_compress_size, sizeof( write_compress_size ) );
        bin_stream->write( compress_buffer, write_compress_size );

        bin_stream->flush();

        Helper::closeOutputStreamFile( m_fileGroupDev, bin_stream );

        return bin_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XmlToBinDecoder::rewind()
    {
        m_stream->rewind();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XmlToBinDecoder::seek( float _time )
    {
        MENGINE_UNUSED( _time );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    float XmlToBinDecoder::tell() const
    {
        return 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
}
