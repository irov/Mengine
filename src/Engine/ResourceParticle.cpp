#include "ResourceParticle.h"

#include "Metacode/Metacode.h"

#include "ResourceImageDefault.h"

#include "Interface/ResourceInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/NodeInterface.h"

#include "Logger/Logger.h"

#include "Consts.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ResourceParticle::ResourceParticle()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceParticle::setFilePath( const FilePath& _filePath )
	{
		m_filePath = _filePath;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath& ResourceParticle::getFilePath() const
	{
		return m_filePath;
	}
    //////////////////////////////////////////////////////////////////////////
    void ResourceParticle::setConverterType( const ConstString & _converterType )
    {
        m_converterType = _converterType;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceParticle::getConverterType() const
    {
        return m_converterType;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceParticle::addResourceImage( const ResourceImagePtr & _resourceImage )
    {
        m_resourceImages.emplace_back( _resourceImage );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceParticle::getResourceImageCount() const
    {
        TVectorResourceImages::size_type resourceImageCount = m_resourceImages.size();

        return (uint32_t)resourceImageCount;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & ResourceParticle::getResourceImage( uint32_t _index ) const
    {
        const ResourceImagePtr & resourceImage = m_resourceImages[_index];

        return resourceImage;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceParticle::_loader( const Metabuf::Metadata * _meta )
	{
		const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceParticle * metadata
			= static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceParticle *>(_meta);

        m_filePath = metadata->get_File_Path();
		metadata->get_File_Converter( &m_converterType );

		uint32_t atlasCount = metadata->get_AtlasCount_Value();

		m_resourceImages.resize( atlasCount );

		const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceParticle::VectorMeta_Atlas & includes_atlas = metadata->get_Includes_Atlas();

		for( Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceParticle::VectorMeta_Atlas::const_iterator
			it = includes_atlas.begin(),
			it_end = includes_atlas.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas & atlas = *it;

			uint32_t index = atlas.get_Index();
			const ConstString & resourceName = atlas.get_ResourceName();

            ResourceImagePtr resourceImage = RESOURCE_SERVICE()
                ->getResourceReferenceT<ResourceImagePtr>( resourceName );

            if( resourceImage == nullptr )
            {
                LOGGER_ERROR("ResourceParticle::_loader %s container %s can't get atlas image %s"
                    , this->getName().c_str()
                    , m_filePath.c_str()
                    , resourceName.c_str()
                    );

                return false;
            }

			m_resourceImages[index] = resourceImage;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceParticle::_convert()
	{
		bool result = this->convertDefault2_( m_converterType, m_filePath, m_filePath );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceParticle::_isValid() const
	{
		if( PARTICLE_SERVICE2()->isAvailable() == false )
		{
			return true;
		}
   
		const ConstString & category = this->getCategory();

		ParticleEmitterContainerInterface2Ptr container = PARTICLE_SERVICE2()
			->createEmitterContainerFromFile( category, m_filePath );
		
		if( container == nullptr )
		{
			LOGGER_ERROR("ResourceParticle::_isValid %s can't create container file '%s'"
				, m_name.c_str()
				, m_filePath.c_str() 
				);

			return false;
		}

		if( container->isValid() == false )
		{
			LOGGER_ERROR("ResourceParticle::_isValid %s can't valid container '%s'"
				, m_name.c_str()
				, m_filePath.c_str() 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceParticle::_compile()
	{
		const ConstString & category = this->getCategory();

		ParticleEmitterContainerInterface2Ptr container = PARTICLE_SERVICE2()
			->createEmitterContainerFromFile( category, m_filePath );

		if( container == nullptr )
		{
			LOGGER_ERROR("ResourceParticle::_compile %s can't create container file '%s'"
				, m_name.c_str()
				, m_filePath.c_str()
				);

			return false;
		}

		for( TVectorResourceImages::size_type
			it = 0,
			it_end = m_resourceImages.size();
		it != it_end;
		++it )
		{
			const ResourceImagePtr & resourceImage = m_resourceImages[it];

            if( resourceImage->incrementReference() == false )
            {
                LOGGER_ERROR("ResourceParticle::_compile '%s' file '%s' can't invalid compile resource image '%s'"
                    , m_name.c_str()
                    , m_filePath.c_str()
                    , resourceImage->getName().c_str()
                    );

                return false;
            }

			container->setAtlasResourceImage( it, resourceImage );
		}

		m_container = container;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceParticle::_release()
	{
        for( TVectorResourceImages::const_iterator
            it = m_resourceImages.begin(),
            it_end = m_resourceImages.end();
            it != it_end;
            ++it )
        {
            const ResourceImagePtr & resourceImage = *it;

            if( resourceImage->decrementReference() == false )
            {
                LOGGER_ERROR("ResourceParticle::_release '%s' file '%s' can't invalid compile resource image '%s'"
                    , m_name.c_str()
                    , m_filePath.c_str()
                    , resourceImage->getName().c_str()
                    );
            }
        }

		m_container = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitterInterfacePtr ResourceParticle::createEmitter()
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR("ResourceParticle::createEmitter %s not compile"
				, this->getName().c_str()
				);

			return nullptr;
		}

        ParticleEmitterInterfacePtr emitter = PARTICLE_SYSTEM2()
            ->createEmitter( m_container );

		return emitter;
	}
	//////////////////////////////////////////////////////////////////////////
}
