#pragma once

#include "Interface/UpdateServiceInterface.h"
#include "Interface/TimepipeInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EUpdateState
    {
        EUS_NORMAL,
        EUS_REMOVE,
    };
    //////////////////////////////////////////////////////////////////////////
    class UpdateService
        : public ServiceBase<UpdateServiceInterface>
        , public TimepipeInterface
    {
    public:
        UpdateService();
        ~UpdateService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    public:
        uint32_t createUpdatater( EUpdateMode _mode, uint32_t _deep, const UpdationInterfacePtr & _updation ) override;
        void replaceUpdatater( uint32_t _id, uint32_t _deep ) override;
        void removeUpdatater( uint32_t _id ) override;

    public:
        void onTimepipe( const UpdateContext * _context ) override;

    protected:
        struct UpdatableProxy
        {
            UpdationInterfacePtr updation;
            EUpdateMode mode;
            uint32_t deep;
            EUpdateState state;
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
        uint32_t placeProxy_( EUpdateMode _mode, uint32_t _deep, const UpdationInterfacePtr & _updatable );
        void updateLeaf_( uint32_t _deep, LeafUpdatable * const _leaf, const UpdateContext * _context );
        LeafUpdatable * getLeafUpdatable( EUpdateMode _mode, uint32_t _deep );
    };
}
