#pragma once

#include "Hero.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HeroBullet
        : public Factorable
    {
        DECLARE_FACTORABLE( HeroBullet );

    public:
        HeroBullet();
        ~HeroBullet() override;

    public:
        void initialize( const HeroPtr & _hero, const mt::vec2f & _startPosition, const mt::vec2f & _startDirection );
        void finalize();

    protected:
        HeroPtr m_hero;

        NodePtr m_node;

        Box2DBodyInterfacePtr m_body;

        mt::vec2f m_startPosition;
        mt::vec2f m_startDirection;

        float m_radius;
        float m_speed;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HeroBullet> HeroBulletPtr;
    //////////////////////////////////////////////////////////////////////////
}