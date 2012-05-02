#	include "ResourceEmitterContainer.h"

#	include "ResourceImplement.h"

#	include "BinParser.h"

#	include "ResourceManager.h"
#	include "ResourceImageDefault.h"
#	include "ParticleEngine.h"
#	include "LogEngine.h"

#	include "ServiceProvider.h"

#	include "Interface/UnicodeInterface.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceEmitterContainer );
	//////////////////////////////////////////////////////////////////////////
	ResourceEmitterContainer::ResourceEmitterContainer()
		: m_container(0)
		, m_emitterStartPosition(0.f)
		, m_emitterPositionOffset(0.f,0.f)
		, m_emitterRelative(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::setFilePath( const WString& _path )
	{
		m_filename = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString& ResourceEmitterContainer::getFilePath() const
	{
		return m_filename;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::setFolderPath( const WString& _folder )
	{
		m_folder = _folder;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString& ResourceEmitterContainer::getFolderPath() const
	{
		return m_folder;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::File_Path, &ResourceEmitterContainer::setFilePath );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Folder_Path, &ResourceEmitterContainer::setFolderPath );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Emitter_Name, &ResourceEmitterContainer::setEmitterName );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::StartPosition_Value, &ResourceEmitterContainer::setEmitterStartPosition );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::PositionOffset_Value, &ResourceEmitterContainer::setEmitterPositionOffset );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::EmitterRelative_Value,  &ResourceEmitterContainer::setEmitterRelative );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitterContainer::_compile()
	{
		const ConstString & category = this->getCategory();

		m_container = ParticleEngine::get()
			->createEmitterContainerFromFile( category, m_filename );

		if( m_container == 0 )
		{
			MENGE_LOG_ERROR( "Image can't create container file '%S'"
				, m_filename.c_str() 
				);

			return false;
		}
		
		UnicodeInterface * unicodeService = ServiceProvider::get()
			->getServiceT<UnicodeInterface>("Unicode");

		const EmitterContainerInterface::TVectorAtlas & atlas = m_container->getAtlas();
		
		for( EmitterContainerInterface::TVectorAtlas::const_iterator
			it = atlas.begin(),
			it_end = atlas.end();
		it != it_end;
		++it )
		{
			
			//String fullname = m_folder + MENGE_FOLDER_DELIM + it->file;
			ConstString name = ConstString(m_name.to_str() + it->file);

			if( ResourceManager::get()
				->hasResource( name ) == false )
			{
				bool w_filename_successful;
				WString filename = unicodeService->utf8ToUnicode( it->file, w_filename_successful );
				WString filepath = m_folder + MENGE_FOLDER_DELIM + filename;

				this->createResource_( name, filepath );
			}
			
			ResourceImageDefault * image = ResourceManager::get()
				->getResourceT<ResourceImageDefault>( name );

			m_atlasImages.push_back( image );
		}

		if( atlas.empty() == true )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::createResource_( const ConstString & _fullname, const WString & _path )
	{
		const ConstString & category = this->getCategory();
		const ConstString & group = this->getGroup();

		ResourceImageDefault * image = ResourceManager::get()
			->createResourceT<ResourceImageDefault>( category, group, _fullname, Consts::get()->c_ResourceImageDefault );

		image->addImagePath( _path, mt::vec2f(-1.f,-1.f) );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::_release()
	{
		for( TVectorAtlasImages::iterator
			it = m_atlasImages.begin(),
			it_end = m_atlasImages.end();
		it != it_end;
		++it)
		{
			ResourceImageDefault * resourceImageDefault = (*it);

			resourceImageDefault->decrementReference();
		}

		m_atlasImages.clear();

		if( m_container != 0 )
		{
			ParticleEngine::get()
				->releaseEmitterContainer( m_container );

			m_container = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault* ResourceEmitterContainer::getAtlasImage( size_t _atlasId )
	{
		return m_atlasImages[_atlasId];
	}
	//////////////////////////////////////////////////////////////////////////
	EmitterContainerInterface * ResourceEmitterContainer::getContainer() const
	{
		return m_container;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::setEmitterName( const ConstString& _name )
	{
		m_emitterName = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& ResourceEmitterContainer::getEmitterName() const
	{
		return m_emitterName;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::setEmitterStartPosition( float _startPosition )
	{
		m_emitterStartPosition = _startPosition;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceEmitterContainer::getEmitterStartPosition() const
	{
		return m_emitterStartPosition;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::setEmitterPositionOffset( const mt::vec2f& _offset )
	{
		m_emitterPositionOffset = _offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& ResourceEmitterContainer::getEmitterPositionOffset() const
	{
		return m_emitterPositionOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::setEmitterRelative( bool _relative )
	{
		m_emitterRelative = _relative;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitterContainer::getEmitterRelative() const
	{
		return m_emitterRelative;
	}
	//////////////////////////////////////////////////////////////////////////
}
