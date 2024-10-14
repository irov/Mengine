#include "HeroWeaponController.h"

#include "Interface/PlayerServiceInterface.h"

#include "Plugins/GOAPPlugin/Tasks/GOAPCook.h"

#include "Hero.h"
#include "HeroBullet.h"

#include "Kernel/InputServiceHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HeroWeaponController::HeroWeaponController()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HeroWeaponController::~HeroWeaponController()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HeroWeaponController::initialize( const HeroPtr & _hero )
    {
        m_hero = _hero;

        m_factoryBullet = Helper::makeFactoryPool<HeroBullet, 64>( MENGINE_DOCUMENT_FACTORABLE );

        SchedulerInterfacePtr scheduler = PLAYER_SERVICE()
            ->createScheduler( MENGINE_DOCUMENT_FACTORABLE );

        m_scheduler = scheduler;

        GOAP::SourceInterfacePtr source = GOAP_SERVICE()
            ->makeSource();

        auto fn = [this]( const GOAP::SourceInterfacePtr & _source )
        {
            ValueHolderPtr<InputMouseButtonEvent> click_event = Helper::makeValueHolder<InputMouseButtonEvent>( MENGINE_DOCUMENT_FACTORABLE );

            Cook::addGlobalMouseButton( _source, MC_LBUTTON, true, nullptr, [click_event]( const InputMouseButtonEvent & _event )
            {
                click_event->setValue( _event );
            }, MENGINE_DOCUMENT_FACTORABLE );

            Cook::addDelay( _source, m_scheduler, 200.f, MENGINE_DOCUMENT_FACTORABLE );
            Cook::addFunction( _source, this, &HeroWeaponController::makeBullet, click_event );
            Cook::addDelay( _source, m_scheduler, 500.f, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        };

        Cook::addWhile( source, fn );

        GOAP::ChainInterfacePtr chain = GOAP_SERVICE()
            ->makeChain( source, nullptr, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE );

        chain->run();

        m_chain = chain;
    }
    //////////////////////////////////////////////////////////////////////////
    void HeroWeaponController::finalize()
    {
        m_hero = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryBullet );

        m_factoryBullet = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void HeroWeaponController::makeBullet( const ValueHolderPtr<InputMouseButtonEvent> & _holderEvent )
    {
        InputMouseButtonEvent ev = _holderEvent->getValue();

        mt::vec2f wp;
        PLAYER_SERVICE()
            ->calcGlobalMouseWorldPosition( {ev.x, ev.y}, &wp );

        float radius = m_hero->getRadius();
        const Box2DBodyInterfacePtr & body = m_hero->getBody();

        mt::vec2f position = body->getBodyPosition();

        if( mt::equal_v2_v2( wp, position ) == true )
        {
            //ToDo

            return;
        }

        mt::vec2f bullet_dir;
        mt::dir_v2_v2( &bullet_dir, wp, position );

        mt::vec2f bullet_position = position + bullet_dir * radius;

        HeroBulletPtr bullet = m_factoryBullet->createObject( MENGINE_DOCUMENT_FACTORABLE );

        bullet->initialize( m_hero, bullet_position, bullet_dir );
    }
    //////////////////////////////////////////////////////////////////////////
}