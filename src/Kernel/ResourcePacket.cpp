#include "ResourcePacket.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourcePacket::ResourcePacket()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourcePacket::~ResourcePacket()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePacket::addResource( const ResourcePtr & _resource )
    {
        m_resources.emplace_back( _resource );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePacket::foreachResources( const LambdaResource & _lambda ) const
    {
        for( const ResourcePtr & resource : m_resources )
        {
            _lambda( resource );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePacket::cache()
    {
        for( const ResourcePtr & resource : m_resources )
        {
            resource->cache();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePacket::uncache()
    {
        for( const ResourcePtr & resource : m_resources )
        {
            resource->uncache();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}