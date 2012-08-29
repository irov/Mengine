#	include "ResourceEmitterContainer.h"

#	include "ResourceImplement.h"

#	include "Metacode.h"

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
	void ResourceEmitterContainer::loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceEmitterContainer * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceEmitterContainer *>(_meta);

        metadata->swap_File_Path( m_filename );
        metadata->swap_Folder_Path( m_folder );
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
		
		UnicodeServiceInterface * unicodeService = ServiceProvider::get()
			->getServiceT<UnicodeServiceInterface>("UnicodeService");

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
				WString filepath = m_folder + MENGE_FOLDER_RESOURCE_DELIM + filename;

				this->createResource_( name, filepath );
			}
			
			ResourceImageDefault * image = ResourceManager::get()
				->getResourceT<ResourceImageDefault>( name );

			if( image == NULL )
			{
				return false;
			}

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

		image->setImagePath( _path );
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
}
