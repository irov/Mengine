#pragma once

#include "Interface/TimepipeInterface.h"

#include "Plugins/Box2DPlugin/Box2DInterface.h"

#include "Unit.h"

#include "HeroMoveController.h"
#include "HeroWeaponController.h"

#include "Kernel/ValueFollowerAcceleration.h"
#include "Kernel/Factorable.h"
#include "Kernel/Node.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Hero
        : public Unit
        , public TimepipeInterface
    {
    public:
        Hero();
        ~Hero() override;
          
    public:
        void initialize( const Box2DWorldInterfacePtr & _world, const NodePtr & _root, const mt::vec2f & _position, const jpp::object & _data ) override;
        void finalize() override;

    public:
        void onTimepipe( const UpdateContext * _contet ) override;

    public:
        HeroMoveControllerPtr m_moveController;
        HeroWeaponControllerPtr m_weaponController;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Hero, Unit> HeroPtr;
    //////////////////////////////////////////////////////////////////////////
}