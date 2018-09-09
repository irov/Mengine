#pragma once

#include "Interface/UpdateInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UpdateService
        : public ServiceBase<UpdateServiceInterface>
    {
    public:
        UpdateService();
        ~UpdateService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        uint32_t createUpdatater( uint32_t _mode, uint32_t _leaf, const UpdationInterfacePtr & _updation ) override;
        void removeUpdatater( uint32_t _id ) override;

    public:
        void update( const UpdateContext * _context ) override;

    protected:
        struct UpdatableProxy
        {
            UpdationInterfacePtr updation;
            uint32_t state;
        };

        typedef Vector<UpdatableProxy> VectorUpdatableProxies;
        VectorUpdatableProxies m_proxies;
                
        typedef Vector<uint32_t> VectorUpdatableIndecies;
        VectorUpdatableIndecies m_proxyFrees;

        struct LeafUpdatable
        {
            VectorUpdatableIndecies indecies;
            VectorUpdatableIndecies indeciesAdd;
        };
        
        typedef Vector<LeafUpdatable> LeafUpdatables;
        LeafUpdatables m_leafs;

        LeafUpdatables m_beforeLeaf;
        LeafUpdatables m_afterLeaf;

    protected:
        uint32_t placeProxy_( const UpdationInterfacePtr & _updatable );
        void updateLeaf_( LeafUpdatable & _leaf, const UpdateContext * _context );
    };
}
