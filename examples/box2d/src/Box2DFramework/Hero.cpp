#include "Hero.h"

#include "Plugins/GraphicsPlugin/GraphicsInterface.h"

#include "HeroMoveController.h"

#include "Kernel/TimepipeHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/ColorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Hero::Hero()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Hero::~Hero()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Hero::initialize( const Box2DWorldInterfacePtr & _world, const NodePtr & _root, const mt::vec2f & _position, const jpp::object & _data )
    {
        Unit::initialize( _world, _root, _position, _data );

        m_moveController = Helper::makeFactorableUnique<HeroMoveController>( MENGINE_DOCUMENT_FUNCTION );
        m_moveController->initialize( HeroPtr::from( this ) );

        m_weaponController = Helper::makeFactorableUnique<HeroWeaponController>( MENGINE_DOCUMENT_FUNCTION );
        m_weaponController->initialize( HeroPtr::from( this ) );

        Helper::addTimepipe( TimepipeInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Hero::finalize()
    {
        Helper::removeTimepipe( TimepipeInterfacePtr::from( this ) );

        m_moveController->finalize();
        m_moveController = nullptr;

        m_weaponController->finalize();
        m_weaponController = nullptr;

        Unit::finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void Hero::onTimepipe( const UpdateContext * _contet )
    {
        m_moveController->update( _contet );
    }
    //////////////////////////////////////////////////////////////////////////
}