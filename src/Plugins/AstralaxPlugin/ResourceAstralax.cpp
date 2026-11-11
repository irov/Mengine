#include "ResourceAstralax.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceAstralax::ResourceAstralax()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceAstralax::~ResourceAstralax()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceAstralax::setResourceImages( const VectorResourceImages & _resourceImages )
    {
        m_resourceImages = _resourceImages;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceAstralax::getResourceImageCount() const
    {
        VectorResourceImages::size_type resourceImageCount = m_resourceImages.size();

        return (uint32_t)resourceImageCount;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & ResourceAstralax::getResourceImage( uint32_t _index ) const
    {
        const ResourceImagePtr & resourceImage = m_resourceImages[_index];

        return resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceAstralax::_compile()
    {
        const ContentInterfacePtr & content = this->getContent();

        AstralaxEmitterContainerInterfacePtr container = ASTRALAX_SERVICE()
            ->createEmitterContainerFromFile( content, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( container, "resource '%s' can't create container file '%s'"
            , this->getName().c_str()
            , Helper::getContentFullPath( this->getContent() ).c_str()
        );

        MENGINE_ASSERTION_FATAL( container->getAtlasResourceImageCount() == m_resourceImages.size(), "resource '%s' container '%s' has different image count '%u != %zu'"
            , this->getName().c_str()
            , Helper::getContentFullPath( this->getContent() ).c_str()
            , container->getAtlasResourceImageCount()
            , m_resourceImages.size()
        );

        uint32_t resourceImageCount = this->getResourceImageCount();

        for( uint32_t index = 0; index != resourceImageCount; ++index )
        {
            const ResourceImagePtr & resourceImage = m_resourceImages[index];

            if( container->setAtlasResourceImage( index, resourceImage ) == false )
            {
                LOGGER_ERROR( "resource astralax '%s' file '%s' invalid set atlas resource image '%s'"
                    , this->getName().c_str()
                    , Helper::getContentFullPath( this->getContent() ).c_str()
                    , resourceImage->getName().c_str()
                );

                return false;
            }
        }

        for( uint32_t index = 0; index != resourceImageCount; ++index )
        {
            const ResourceImagePtr & resourceImage = m_resourceImages[index];

            if( resourceImage->compile() == false )
            {
                LOGGER_ERROR( "resource astralax '%s' file '%s' can't invalid compile resource image '%s'"
                    , this->getName().c_str()
                    , Helper::getContentFullPath( this->getContent() ).c_str()
                    , resourceImage->getName().c_str()
                );

                this->releaseResourceImages_( index );

                return false;
            }
        }

        m_container = container;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceAstralax::_release()
    {
        uint32_t resourceImageCount = this->getResourceImageCount();

        this->releaseResourceImages_( resourceImageCount );

        m_container = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceAstralax::releaseResourceImages_( uint32_t _count )
    {
        for( uint32_t index = 0; index != _count; ++index )
        {
            const ResourceImagePtr & resourceImage = m_resourceImages[index];

            resourceImage->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitterInterfacePtr ResourceAstralax::createEmitter( const DocumentInterfacePtr & _doc )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "resource astralax '%s' not compile"
                , this->getName().c_str()
            );

            return nullptr;
        }

        AstralaxEmitterInterfacePtr emitter = ASTRALAX_SERVICE()
            ->createEmitter( m_container, _doc );

        return emitter;
    }
    //////////////////////////////////////////////////////////////////////////
}
