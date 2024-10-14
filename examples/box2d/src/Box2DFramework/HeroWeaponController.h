#pragma once

#include "Interface/SchedulerInterface.h"
#include "Interface/FactoryInterface.h"

#include "Plugins/GOAPPlugin/GOAPInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/InputEvent.h"
#include "Kernel/ValueHolder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Hero, class Unit> HeroPtr;
    //////////////////////////////////////////////////////////////////////////
    class HeroWeaponController
        : public Factorable
    {
    public:
        HeroWeaponController();
        ~HeroWeaponController() override;

    public:
        void initialize( const HeroPtr & _hero );
        void finalize();

    protected:
        void makeBullet( const ValueHolderPtr<InputMouseButtonEvent> & _holderEvent );

    protected:
        HeroPtr m_hero;

        FactoryInterfacePtr m_factoryBullet;

        SchedulerInterfacePtr m_scheduler;

        GOAP::ChainInterfacePtr m_chain;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HeroWeaponController> HeroWeaponControllerPtr;
    //////////////////////////////////////////////////////////////////////////
}