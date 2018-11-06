#include "ResourceAstralax.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/NodeInterface.h"

#include "Kernel/Logger.h"

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
    bool ResourceAstralax::_convert()
    {
        bool result = this->convertDefault2_( m_converterType, m_filePath, m_filePath );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceAstralax::_compile()
    {
        const ConstString & name = this->getName();
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        AstralaxEmitterContainerInterfacePtr container = ASTRALAX_SERVICE()
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
    AstralaxEmitterInterfacePtr ResourceAstralax::createEmitter()
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "resource %s not compile"
                , this->getName().c_str()
            );

            return nullptr;
        }

        AstralaxEmitterInterfacePtr emitter = ASTRALAX_SYSTEM()
            ->createEmitter( m_container );

        return emitter;
    }
    //////////////////////////////////////////////////////////////////////////
}
