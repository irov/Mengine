#include "ResourceAstralax.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

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

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        AstralaxEmitterContainerInterfacePtr container = ASTRALAX_SERVICE()
            ->createEmitterContainerFromFile( fileGroup, filePath, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( container, "resource '%s' can't create container file '%s'"
            , this->getName().c_str()
            , Helper::getFileGroupFullPath( this->getContent()->getFileGroup(), this->getContent()->getFilePath() )
        );

        MENGINE_ASSERTION_FATAL( container->getAtlasResourceImageCount() == m_resourceImages.size(), "resource '%s' container '%s' has different image count '%u != %zu'"
            , this->getName().c_str()
            , Helper::getFileGroupFullPath( this->getContent()->getFileGroup(), this->getContent()->getFilePath() )
            , container->getAtlasResourceImageCount()
            , m_resourceImages.size()
        );

        uint32_t iterator = 0U;
        for( const ResourceImagePtr & resourceImage : m_resourceImages )
        {
            if( resourceImage->compile() == false )
            {
                LOGGER_ERROR( "resource '%s' file '%s' can't invalid compile resource image '%s'"
                    , this->getName().c_str()
                    , this->getContent()->getFilePath().c_str()
                    , resourceImage->getName().c_str()
                );

                return false;
            }

            if( container->setAtlasResourceImage( iterator, resourceImage ) == false )
            {
                LOGGER_ERROR( "resource '%s' file '%s' invalid set atlas resource image '%s'"
                    , this->getName().c_str()
                    , this->getContent()->getFilePath().c_str()
                    , resourceImage->getName().c_str()
                );

                return false;
            }

            ++iterator;
        }

        m_container = container;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceAstralax::_release()
    {
        for( const ResourceImagePtr & resourceImage : m_resourceImages )
        {
            resourceImage->release();
        }

        m_container = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitterInterfacePtr ResourceAstralax::createEmitter( const DocumentInterfacePtr & _doc )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "resource '%s' not compile"
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
