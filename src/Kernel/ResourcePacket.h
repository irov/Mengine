#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/VectorResources.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourcePacket
        : public Factorable
    {
    public:
        ResourcePacket();
        ~ResourcePacket() override;

    public:
        void addResource( const ResourcePtr & _resource );

    public:
        typedef Lambda<void( const ResourcePtr & )> LambdaResource;
        void foreachResources( const LambdaResource & _lambda ) const;

    public:
        void cache();
        void uncache();

    protected:
        VectorResources m_resources;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourcePacket> ResourcePacketPtr;
    //////////////////////////////////////////////////////////////////////////
}