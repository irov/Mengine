#include "HeroMoveController.h"

#include "Hero.h"

#include "Kernel/InputServiceHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HeroMoveController::HeroMoveController()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HeroMoveController::~HeroMoveController()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HeroMoveController::initialize( const HeroPtr & _hero )
    {
        m_hero = _hero;        
    }
    //////////////////////////////////////////////////////////////////////////
    void HeroMoveController::finalize()
    {
        m_hero = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void HeroMoveController::update( const UpdateContext * _contet )
    {
        MENGINE_UNUSED( _contet );

        mt::vec2f velocity( 0.f, 0.f );

        if( Helper::isKeyDown( KC_W ) == true )
        {
            velocity += mt::vec2f( 0.f, -1.f );
        }

        if( Helper::isKeyDown( KC_S ) == true )
        {
            velocity += mt::vec2f( 0.f, 1.f );
        }

        if( Helper::isKeyDown( KC_A ) == true )
        {
            velocity += mt::vec2f( -1.f, 0.f );
        }

        if( Helper::isKeyDown( KC_D ) == true )
        {
            velocity += mt::vec2f( 1.f, 0.f );
        }

        if( mt::is_zero_v2( velocity ) == true )
        {
            m_hero->setMoveDirection( mt::vec2f( 0.f, 0.f ) );

            return;
        }

        mt::vec2f norm_velocity = mt::norm_v2( velocity );

        m_hero->setMoveDirection( norm_velocity );
    }
    //////////////////////////////////////////////////////////////////////////
}