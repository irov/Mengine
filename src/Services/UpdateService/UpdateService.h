#pragma once

#include "Interface/UpdateServiceInterface.h"

#include "Kernel/ServiceBase.h"

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
        void _stopService() override;

    public:
        uint32_t createUpdatater( uint32_t _mode, uint32_t _deep, const UpdationInterfacePtr & _updation ) override;
        void replaceUpdatater( uint32_t _id, uint32_t _deep ) override;
        void removeUpdatater( uint32_t _id ) override;

    public:
        void update( const UpdateContext * _context ) override;

    protected:
        struct UpdatableProxy
        {
            UpdationInterfacePtr updation;
            uint32_t mode;
            uint32_t deep;
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
        uint32_t placeProxy_( uint32_t _mode, uint32_t _deep, const UpdationInterfacePtr & _updatable );
        void updateLeaf_( uint32_t _deep, LeafUpdatable & _leaf, const UpdateContext * _context );
        LeafUpdatable * getLeafUpdatable( uint32_t _mode, uint32_t _deep );
    };
}
