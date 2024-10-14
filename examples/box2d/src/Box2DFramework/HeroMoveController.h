#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/UpdateContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Hero, class Unit> HeroPtr;
    //////////////////////////////////////////////////////////////////////////
    class HeroMoveController
        : public Factorable
    {
    public:
        HeroMoveController();
        ~HeroMoveController() override;

    public:
        void initialize( const HeroPtr & _hero );
        void finalize();

    public:
        void update( const UpdateContext * _contet );

    protected:
        HeroPtr m_hero;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HeroMoveController> HeroMoveControllerPtr;
    //////////////////////////////////////////////////////////////////////////
}