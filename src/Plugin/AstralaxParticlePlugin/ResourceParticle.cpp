#include "ResourceParticle.h"

#include "Metacode/Metacode.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/NodeInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceParticle::ResourceParticle()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceParticle::~ResourceParticle()
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
        VectorResourceImages::size_type resourceImageCount = m_resourceImages.size();

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
                ->getResourceReference( resourceName );

            if( resourceImage == nullptr )
            {
                LOGGER_ERROR( "resource %s container %s can't get atlas image %s"
                    , this->getName().c_str()
                    , this->getFilePath().c_str()
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
    bool ResourceParticle::_compile()
    {
        const ConstString & name = this->getName();
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        ParticleEmitterContainerInterface2Ptr container = PARTICLE_SERVICE2()
            ->createEmitterContainerFromFile( fileGroup, m_filePath, name );

        if( container == nullptr )
        {
            LOGGER_ERROR( "resource %s can't create container file '%s'"
                , this->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        uint32_t iterator = 0U;
        for( const ResourceImagePtr & resourceImage : m_resourceImages )
        {
            if( resourceImage->incrementReference() == false )
            {
                LOGGER_ERROR( "resource '%s' file '%s' can't invalid compile resource image '%s'"
                    , this->getName().c_str()
                    , this->getFilePath().c_str()
                    , resourceImage->getName().c_str()
                );

                return false;
            }

            container->setAtlasResourceImage( iterator, resourceImage );

            ++iterator;
        }

        m_container = container;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceParticle::_release()
    {
        for( const ResourceImagePtr & resourceImage : m_resourceImages )
        {
            if( resourceImage->decrementReference() == false )
            {
                LOGGER_ERROR( "resource '%s' file '%s' can't invalid compile resource image '%s'"
                    , this->getName().c_str()
                    , this->getFilePath().c_str()
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
            LOGGER_ERROR( "resource %s not compile"
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
