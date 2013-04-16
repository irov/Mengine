#	include "ResourceImageDefault.h"

#	include "Kernel/ResourceImplement.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"
#	include "Interface/RenderSystemInterface.h"

#   include "Logger/Logger.h"

#   include "Metacode.h"

#	include "Core/ConstString.h"
#	include "Core/String.h"

#	include <cstdio>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageDefault );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault::ResourceImageDefault()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceImageDefault::getFileName() const
	{
		return m_fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageDefault::getCodecType() const
	{
		return m_codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_isValid() const
	{
		const ConstString & category = this->getCategory();

		bool exist = FILE_SERVICE(m_serviceProvider)
			->existFile( category, m_fileName, NULL );
				
		if( exist == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceImageDefault * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageDefault *>(_meta);

        m_uv = mt::vec4f(0.f, 0.f, 1.f, 1.f);
        m_maxSize = mt::vec2f(-1.f, -1.f);
        m_size = mt::vec2f(-1.f, -1.f);
        m_isAlpha = true;
        m_isUVRotate = false;
        m_wrapX = false;
        m_wrapY = false;
        
        metadata->swap_File_Path( m_fileName );        
        metadata->swap_File_Codec( m_codecType );        

        metadata->get_File_UV( m_uv );
        metadata->get_File_Alpha( m_isAlpha );
        metadata->get_File_Rotate( m_isUVRotate );
        metadata->get_File_WrapX( m_wrapX );
        metadata->get_File_WrapY( m_wrapY );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{	
		const ConstString & category = this->getCategory();

		if( m_codecType.empty() == true )
		{
			m_codecType = this->getCodec_( m_fileName );
		}
			
		if( this->loadImageFrame_( category, m_fileName, m_codecType ) == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::setImagePath( const FilePath& _imagePath )
	{
        m_fileName = _imagePath;
        m_codecType = this->getCodec_(_imagePath);

        m_texture = NULL;
        m_uv = mt::vec4f(0.f,0.f,1.f,1.f);
		m_isAlpha = true;
        m_isUVRotate = false;
		m_wrapX = false;
		m_wrapY = false;
	}
	/////////////////////////////////////////////////////////////////////////
}
