#pragma once

#include "Interface/UpdateServiceInterface.h"
#include "Interface/TimepipeInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<UpdationInterface *> VectorUpdatableIndecies;
    //////////////////////////////////////////////////////////////////////////
    struct LeafUpdatable
    {
        VectorUpdatableIndecies indecies;
        VectorUpdatableIndecies indeciesAdd;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<LeafUpdatable> LeafUpdatables;
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
        void placeUpdatater( UpdationInterface * _updation ) override;
        void removeUpdatater( UpdationInterface * _updation ) override;

    public:
        void onTimepipe( const UpdateContext * _context ) override;

    protected:
        LeafUpdatables m_beforeLeafs;
        LeafUpdatables m_leafs;
        LeafUpdatables m_afterLeafs;

    protected:
        bool findUpdatater_( const UpdationInterface * _updation ) const;
        void updateLeaf_( uint32_t _deep, LeafUpdatable * const _leaf, const UpdateContext * _context );
        LeafUpdatable * getLeafUpdatable( EUpdateMode _mode, uint32_t _deep );
    };
}
