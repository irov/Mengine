#pragma once

#include "Interface/UpdateServiceInterface.h"
#include "Interface/TimepipeInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

#include "Config/UniqueId.h"

namespace Mengine
{
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
        void placeUpdatater( const UpdationInterfacePtr & _updation ) override;

    public:
        void onTimepipe( const UpdateContext * _context ) override;

    protected:
        typedef Vector<UpdationInterfacePtr> VectorUpdatableIndecies;

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
        void updateLeaf_( uint32_t _deep, LeafUpdatable * const _leaf, const UpdateContext * _context );
        void finalizeLeaf_( LeafUpdatables & _leafs );
        LeafUpdatable * getLeafUpdatable( EUpdateMode _mode, uint32_t _deep );
    };
}
