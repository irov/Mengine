#include "ResourceAstralax.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

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
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();
        const FilePath & filePath = this->getFilePath();

        AstralaxEmitterContainerInterfacePtr container = ASTRALAX_SERVICE()
            ->createEmitterContainerFromFile( fileGroup, filePath, MENGINE_DOCUMENT( "ResourceAstralax::compile '%s'", this->getName().c_str() ) );

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

            if( container->setAtlasResourceImage( iterator, resourceImage ) == false )
            {
                LOGGER_ERROR( "resource '%s' file '%s' invalid set atlas resource image '%s'"
                    , this->getName().c_str()
                    , this->getFilePath().c_str()
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
    AstralaxEmitterInterfacePtr ResourceAstralax::createEmitter( const Char * _doc )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "resource %s not compile"
                , this->getName().c_str()
            );

            return nullptr;
        }

        AstralaxEmitterInterfacePtr emitter = ASTRALAX_SYSTEM()
            ->createEmitter( m_container, _doc );

        return emitter;
    }
    //////////////////////////////////////////////////////////////////////////
}
