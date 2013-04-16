#	include "ResourceEmitterContainer.h"
#	include "Kernel/ResourceImplement.h"

#	include "ResourceImageDefault.h"

#	include "Metacode.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

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
	void ResourceEmitterContainer::setFilePath( const FilePath& _path )
	{
		m_filename = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath& ResourceEmitterContainer::getFilePath() const
	{
		return m_filename;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::setFolderPath( const FilePath& _folder )
	{
		m_folder = _folder;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath& ResourceEmitterContainer::getFolderPath() const
	{
		return m_folder;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitterContainer::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceEmitterContainer * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceEmitterContainer *>(_meta);

        metadata->swap_File_Path( m_filename );
        metadata->swap_Folder_Path( m_folder );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitterContainer::_compile()
	{
		const ConstString & category = this->getCategory();

		m_container = PARTICLE_SERVICE(m_serviceProvider)
			->createEmitterContainerFromFile( category, m_filename );

		if( m_container == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "Image can't create container file '%s'"
				, m_filename.c_str() 
				);

			return false;
		}
		
		const EmitterContainerInterface::TVectorAtlas & atlas = m_container->getAtlas();
		
		for( EmitterContainerInterface::TVectorAtlas::const_iterator
			it = atlas.begin(),
			it_end = atlas.end();
		it != it_end;
		++it )
		{
			const FilePath & filepath = it->file;

			//String fullname = m_folder + MENGE_FOLDER_DELIM + it->file;
            static String cache_name;
            cache_name.assign( m_name.c_str(), m_name.size() );
            cache_name.append( filepath.c_str(), filepath.size() );

			ConstString name = Helper::stringizeString( m_serviceProvider, cache_name );

			if( RESOURCE_SERVICE(m_serviceProvider)
				->hasResource( name ) == false )
			{				
                static String cache_path;
                cache_path.assign( m_folder.c_str(), m_folder.size() );
                cache_path += MENGE_FOLDER_RESOURCE_DELIM;
                cache_path.append( filepath.c_str(), filepath.size() );

                FilePath filepath = Helper::stringizeString( m_serviceProvider, cache_path );

				this->createResource_( name, filepath );
			}
			
			ResourceImageDefault * image = RESOURCE_SERVICE(m_serviceProvider)
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
	void ResourceEmitterContainer::createResource_( const ConstString & _fullname, const FilePath & _path )
	{
		const ConstString & category = this->getCategory();
		const ConstString & group = this->getGroup();

		ResourceImageDefault * image = RESOURCE_SERVICE(m_serviceProvider)
			->createResourceT<ResourceImageDefault>( category, group, _fullname, CONST_STRING(m_serviceProvider, ResourceImageDefault) );

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
			PARTICLE_SERVICE(m_serviceProvider)
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
