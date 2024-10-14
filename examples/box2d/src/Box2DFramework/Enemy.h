#pragma once

#include "Interface/TimepipeInterface.h"

#include "Plugins/Box2DPlugin/Box2DInterface.h"

#include "HeroMoveController.h"
#include "HeroWeaponController.h"

#include "Kernel/ValueFollowerAcceleration.h"
#include "Kernel/Factorable.h"
#include "Kernel/Node.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Enemy
        : public Factorable
        , public TimepipeInterface
    {
    public:
        Enemy();
        ~Enemy() override;

    public:
        void initialize( const Box2DWorldInterfacePtr & _world, const NodePtr & _root );
        void finalize();

    public:
        void onTimepipe( const UpdateContext * _contet ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Enemy, Unit> EnemyPtr;
    //////////////////////////////////////////////////////////////////////////
}