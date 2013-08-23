#	include "ParticleConverterPTCToPTZ.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"

#	include "Logger/Logger.h"
#   include "Core/FilePath.h"
#   include "Config/Blobject.h"

#   include "WindowsLayer/WindowsIncluder.h"

namespace Menge
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	ParticleConverterPTCToPTZ::ParticleConverterPTCToPTZ( ServiceProviderInterface * _serviceProvider )
		: ParticleConverter(_serviceProvider)
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	ParticleConverterPTCToPTZ::~ParticleConverterPTCToPTZ()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool ParticleConverterPTCToPTZ::initialize()
	{
        m_convertExt = ".ptz";

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    const String & ParticleConverterPTCToPTZ::getConvertExt() const
    {
        return m_convertExt;
    }
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool ParticleConverterPTCToPTZ::convert()
	{
        FileGroupInterface * fileGroup;
        if( FILE_SERVICE(m_serviceProvider)->hasFileGroup( m_options.pakName, &fileGroup ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ParticleConverterPTCToPTZ::convert_: not found file group '%s'"
                , m_options.pakName.c_str()
                );

            return false;
        }

        const ConstString & pakPath = fileGroup->getPath();            

        ConstString full_input = concatenationFilePath( m_serviceProvider, pakPath, m_options.inputFileName );
        ConstString full_output = concatenationFilePath( m_serviceProvider, pakPath, m_options.outputFileName );

        ConstString c_dev = Helper::stringizeString( m_serviceProvider, "dev" );

        InputStreamInterfacePtr input = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( c_dev, full_input );

        size_t data_size = input->size();

        static TBlobject s_buffer;
        s_buffer.resize( data_size );

        input->read( &s_buffer[0], data_size );

        size_t archive_size = ARCHIVE_SERVICE(m_serviceProvider)
            ->compressBound( data_size );

        if( archive_size == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "ParticleConverterPTCToPTZ::convert_: %s invalid compressBound %d"
                , m_options.inputFileName.c_str()
                , data_size
                );

            return false;
        }

        TBlobject archive_blob(archive_size);

        size_t comress_size;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->compress( &archive_blob[0], archive_size, comress_size, &s_buffer[0], data_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ParticleConverterPTCToPTZ::convert_: %s invalid compress"
                , m_options.inputFileName.c_str()
                );

            return false;
        }

        OutputStreamInterfacePtr output = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( c_dev, full_output );

        output->write( &data_size, sizeof(data_size) );
        output->write( &comress_size, sizeof(comress_size) );
        output->write( &archive_blob[0], comress_size );

        return true;
	}
}