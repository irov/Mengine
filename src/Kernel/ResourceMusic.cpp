#include "ResourceMusic.h"

#include "Interface/FileSystemInterface.h"
#include "Interface/CodecInterface.h"

#include "Metacode/Metacode.h"

#include "Logger/Logger.h"

#include "Core/String.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ResourceMusic::ResourceMusic()
		: m_volume(1.f)
		, m_external(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceMusic::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceMusic::getCodec() const
	{
		return m_codec;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceMusic::getVolume() const
	{
		return m_volume;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMusic::isExternal() const
	{
		return m_external;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMusic::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMusic * metadata 
			= static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMusic *>(_meta);

        m_path = metadata->get_File_Path();

		if( metadata->get_File_Codec( &m_codec ) == false )
		{
			m_codec = CODEC_SERVICE()
				->findCodecType( m_path );
		}

		metadata->get_File_Converter( &m_converter );
		metadata->get_File_External( &m_external );
		metadata->get_DefaultVolume_Value( &m_volume );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMusic::_isValid() const
	{
		if( m_external == true )
		{
			return true;
		}
		
		const ConstString & category = this->getCategory();

		if( FILE_SERVICE()
			->existFile( category, m_path, nullptr ) == false )
		{
			LOGGER_ERROR("ResourceMusic::_loader: '%s' music '%s' not exist"
				, this->getName().c_str()
				, m_path.c_str()
				);

			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMusic::_convert()
	{
		bool result = this->convertDefault_( m_converter, m_path, m_path, m_codec );

		return result;
	}
}
