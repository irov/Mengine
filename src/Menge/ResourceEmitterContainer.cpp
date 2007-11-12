#	include "ResourceEmitterContainer.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "ParticleEngine.h"
#	include "LogEngine.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceEmitterContainer )
	//////////////////////////////////////////////////////////////////////////
	ResourceEmitterContainer::ResourceEmitterContainer( const std::string & _name )
		: ResourceReference( _name )
		, m_image( 0 )
		, m_container( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::loader( TiXmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "File", "Path", m_filename );
			XML_CHECK_VALUE_NODE( "Image", "Path", m_fileImage );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitterContainer::_compile()
	{
		FileDataInterface * fileData = Holder<FileEngine>::hostage()->openFile( m_filename );

		if( fileData == 0 )
		{
			MENGE_LOG( "Error: Emitter can't open resource file '%s'\n", m_filename.c_str() );
			return 0;
		}

		size_t buff_size = fileData->size();

		std::vector<char> buffer( buff_size );
		fileData->read( &buffer[0], buff_size );

		m_container = Holder<ParticleEngine>::hostage()->createEmitterContainerFromMemory( &buffer[0] );

		Holder<FileEngine>::hostage()->closeFile( fileData );

		if( m_container == 0 )
		{
			MENGE_LOG( "Error: Image can't create container file '%s'", m_filename.c_str() );
			return false;
		}

		// разобратся с текстурами!! FIX THIS
		FileDataInterface * fileDataImage = Holder<FileEngine>::hostage()->openFile( m_fileImage );

		if( fileDataImage == 0 )
		{
			MENGE_LOG( "Error: Image can't open resource file '%s'", m_fileImage.c_str() );
			return false;
		}

		buff_size = fileDataImage->size();
		
		std::vector<char> _buff( buff_size );

		fileDataImage->read( &_buff[0], buff_size );

		TextureDesc textureDesc;

		textureDesc.buffer = &_buff[0];
		textureDesc.size = buff_size;
		textureDesc.name = m_fileImage.c_str();
		textureDesc.filter = 1;

		m_image = Holder<RenderEngine>::hostage()->loadImage( textureDesc );

		Holder<FileEngine>::hostage()->closeFile( fileDataImage );

		if( m_image == 0 )
		{
			MENGE_LOG( "Error: Image can't loaded '%s'", m_fileImage.c_str() );
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::_release()
	{
		Holder<RenderEngine>::hostage()
			->releaseImage( m_image );
	}
	//////////////////////////////////////////////////////////////////////////
	const EmitterContainerInterface * ResourceEmitterContainer::getContainer() const
	{
		return m_container;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * ResourceEmitterContainer::getImage() const
	{
		return m_image;
	}
	//////////////////////////////////////////////////////////////////////////
}