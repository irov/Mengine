#include "ResourceMultiTexturepacker.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ContentInterface.h"

#include "Plugins/JSONPlugin/JSONInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/FileStreamHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceMultiTexturepacker::ResourceMultiTexturepacker()
        : m_needStripFrameNameExtension( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMultiTexturepacker::~ResourceMultiTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMultiTexturepacker::addResourceTexturepacker( const ResourcePtr & _resourceTexturepacker )
    {
        m_resourceTexturepackers.emplace_back( _resourceTexturepacker );
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorResources & ResourceMultiTexturepacker::getResourceTexturepackers() const
    {
        return m_resourceTexturepackers;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMultiTexturepacker::findFrame( const ConstString & _name, ResourceImagePtr * const _resourceImage ) const
    {
        ResourceImagePtr resourceImage;

        for( const ResourcePtr & resource : m_resourceTexturepackers )
        {
            UnknownResourceTexturepackerGetterInterface * unknownResourceTexturepacker = resource->getUnknown();

            if( unknownResourceTexturepacker->findFrame( _name, &resourceImage ) == false )
            {
                continue;
            }
        }

        if( resourceImage == nullptr )
        {
            return false;
        }

        if( _resourceImage != nullptr )
        {
            *_resourceImage = resourceImage;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMultiTexturepacker::foreachFrames( const LambdaFrames & _lambdaFrames ) const
    {
        for( const ResourcePtr & resource : m_resourceTexturepackers )
        {
            UnknownResourceTexturepackerGetterInterface * unknownResourceTexturepacker = resource->getUnknown();

            if( unknownResourceTexturepacker->foreachFrames( _lambdaFrames ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMultiTexturepacker::setStripFrameNameExtension( bool _value )
    {
        m_needStripFrameNameExtension = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMultiTexturepacker::getStripFrameNameExtension() const
    {
        return m_needStripFrameNameExtension;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMultiTexturepacker::_compile()
    {
        for( const ResourcePtr & resourceTexturepacker : m_resourceTexturepackers )
        {
            UnknownResourceTexturepackerInterface * unknownResourceTexturepacker = resourceTexturepacker->getUnknown();

            unknownResourceTexturepacker->setStripFrameNameExtension( m_needStripFrameNameExtension );

            if( resourceTexturepacker->compile() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMultiTexturepacker::_release()
    {
        for( const ResourcePtr & resourceTexturepacker : m_resourceTexturepackers )
        {
            resourceTexturepacker->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}