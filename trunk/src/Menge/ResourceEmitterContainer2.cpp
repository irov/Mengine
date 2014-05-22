#	include "ResourceEmitterContainer2.h"

#	include "Kernel/ResourceImplement.h"

#	include "ResourceImageDefault.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceEmitterContainer2 );
	//////////////////////////////////////////////////////////////////////////
	ResourceEmitterContainer2::ResourceEmitterContainer2()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer2::setFilePath( const FilePath& _path )
	{
		m_fileName = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath& ResourceEmitterContainer2::getFilePath() const
	{
		return m_fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitterContainer2::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceEmitterContainer2 * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceEmitterContainer2 *>(_meta);

        metadata->swap_File_Path( m_fileName );
        
		uint32_t atlasCount = metadata->get_AtlasCount_Value();
		
		m_resourceImageNames.reserve( atlasCount );
		m_resourceImages.reserve( atlasCount );

		const Metacode::Meta_DataBlock::Meta_ResourceEmitterContainer2::TVectorMeta_Atlas & includes_atlas = metadata->get_IncludesAtlas();

		for( Metacode::Meta_DataBlock::Meta_ResourceEmitterContainer2::TVectorMeta_Atlas::const_iterator
			it = includes_atlas.begin(),
			it_end = includes_atlas.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_DataBlock::Meta_ResourceEmitterContainer2::Meta_Atlas & atlas = *it;

			uint32_t index = atlas.get_Index();
			const Menge::ConstString & resourceName = atlas.get_ResourceName();

			m_resourceImageNames[index] = resourceName;
		}

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceEmitterContainer2::_isValid() const
    {
        const ConstString & category = this->getCategory();

		ParticleEmitterContainerInterface2Ptr container = PARTICLE_SERVICE2(m_serviceProvider)
            ->createEmitterContainerFromFile( category, m_fileName );

        if( container == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceEmitterContainer2::_isValid %s can't create container file '%s'"
                , m_name.c_str()
                , m_fileName.c_str() 
                );

            return false;
        }

		if( container->isValid() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceEmitterContainer2::_isValid %s can't valid container '%s'"
				, m_name.c_str()
				, m_fileName.c_str() 
				);

			return false;
		}

        for( TVectorResourceImageName::const_iterator
            it = m_resourceImageNames.begin(),
            it_end = m_resourceImageNames.end();
        it != it_end;
        ++it )
        {            
            const ConstString & name = *it;

			if( RESOURCE_SERVICE(m_serviceProvider)
				->hasResource( name, nullptr ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceEmitterContainer2::_isValid %s can't get image resource '%s'"
					, m_name.c_str()
					, name.c_str()
					);

				return false;
			}            
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitterContainer2::_compile()
	{
		const ConstString & category = this->getCategory();

		m_container = PARTICLE_SERVICE2(m_serviceProvider)
			->createEmitterContainerFromFile( category, m_fileName );

		if( m_container == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceEmitterContainer2::_compile %s can't create container file '%s'"
                , m_name.c_str()
				, m_fileName.c_str() 
				);

			return false;
		}
				
		for( TVectorResourceImageName::const_iterator
			it = m_resourceImageNames.begin(),
			it_end = m_resourceImageNames.end();
		it != it_end;
		++it )
		{
			const ConstString & resourceName = *it;

			ResourceImage * resourceImage = RESOURCE_SERVICE(m_serviceProvider)
				->getResourceT<ResourceImage>( resourceName );

			m_resourceImages.push_back( resourceImage );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer2::_release()
	{
		m_container = nullptr;

		for( TVectorResourceImage::const_iterator
			it = m_resourceImages.begin(),
			it_end = m_resourceImages.end();
		it != it_end;
		++it )
		{
			ResourceImage * resource = *it;

			resource->decrementReference();
		}

		m_resourceImages.clear();		
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage * ResourceEmitterContainer2::getAtlasImageResource( size_t _atlasId ) const
	{
        ResourceImage * resource = m_resourceImages[_atlasId];

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceEmitterContainer2::getAtlasImageCount() const
	{
		size_t count = m_resourceImages.size();

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
	const ParticleEmitterContainerInterface2Ptr & ResourceEmitterContainer2::getContainer() const
	{
		return m_container;
	}
	//////////////////////////////////////////////////////////////////////////
}
