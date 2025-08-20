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
        void removeUpdatater( const UpdationInterfacePtr & _updation ) override;

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
        LeafUpdatables m_beforeLeafs;
        LeafUpdatables m_leafs;
        LeafUpdatables m_afterLeafs;

    protected:
        bool findLeafUpdatater_( const LeafUpdatables & _leafs, const UpdationInterfacePtr & _updation ) const;
        bool findUpdatater_( const UpdationInterfacePtr & _updation ) const;
        void updateLeaf_( uint32_t _deep, LeafUpdatable * const _leaf, const UpdateContext * _context );
        LeafUpdatable * getLeafUpdatable( EUpdateMode _mode, uint32_t _deep );
    };
}
