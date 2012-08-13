#	include "ResourceSound.h"

#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "LogEngine.h"
//#	include "SoundEngine.h"
#	include "CodecEngine.h"
#	include "ConverterEngine.h"

#	include "Interface/SoundSystemInterface.h"

#	include "Metacode.h"

#	include "Utils/Core/File.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceSound );
	//////////////////////////////////////////////////////////////////////////
	ResourceSound::ResourceSound()
		: m_isStreamable(false)
		, m_interface(0)
		, m_defaultVolume(1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setPath( const WString& _path )
	{
		m_path = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setCodec( const ConstString& _codec )
	{
		m_codec = _codec;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setConverter( const ConstString& _converter )
	{
		m_converter = _converter;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceSound * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceSound *>(_meta);

        metadata->swap_File_Path( m_path );
        metadata->swap_File_Codec( m_codec );
        metadata->swap_File_Converter( m_converter );

        metadata->get_DefaultVolume_Value( m_defaultVolume );
        metadata->get_IsStreamable_Value( m_isStreamable );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::isStreamable() const
	{
		return m_isStreamable;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & ResourceSound::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::_compile()
	{
		const ConstString & category = this->getCategory();
		
		//perform convertation if we need
		if ( m_converter.empty() == false )
		{
			if( this->convert() == false )
			{
				return false;
			}
		}

		if( m_codec.empty() == true )
		{
			WString codecExt;
			Utils::getFileExt( m_path, codecExt );

			m_codec = CodecEngine::get()
				->findCodecType( codecExt );
		}

		m_interface = Holder<SoundServiceInterface>::get()
			->createSoundBufferFromFile( category, m_path, m_codec, m_isStreamable );

		if( m_interface == 0 )
		{
			MENGE_LOG_INFO( "ResourceSound::_compile: '%s' can't load sound '%S'"
				, this->getName().c_str() 
				, m_path.c_str()
				);

			return false;			
		}

        ////Ops! i did this again ^^
        //this->incrementReference();
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::convert()
	{
		const ConstString & category = this->getCategory();

		ConverterInterface * converter = ConverterEngine::get()
			->createConverterT<ConverterInterface>(m_converter);
		if ( converter == NULL )
		{
			MENGE_LOG_INFO( "ResourceSound::convert: '%s' can't create converter '%s'"
				, getName().c_str() 
				, m_converter.c_str()
				);

			return false;
		}

		ConverterOptions options; 
		options.pakName = category;
		options.inputFileName = m_path;
		
		FileEngine::get()
			->getFileSystemPath( category, options.pakNameFullPath );
			
		converter->setOptions( &options );
		
		if ( converter->convert() == false )
		{
			MENGE_LOG_INFO( "ResourceSound::convert '%s' can't  convert '%s'\n"
				, this->getName().c_str() 
				, m_converter.c_str()
				);

			converter->destroy();

			return false;
		} 
		const ConverterDataInfo * convertResult = converter->getConverterDataInfo();

		m_path = convertResult->outputFileName;

		converter->destroy();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::_release()
	{
		Holder<SoundServiceInterface>::get()
			->releaseSoundBuffer( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface * ResourceSound::getSoundBuffer() const
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceSound::getDefaultVolume() const
	{
		return m_defaultVolume;
	}
}
