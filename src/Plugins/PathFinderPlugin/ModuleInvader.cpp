#include "ModuleInvader.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/PlayerInterface.h"
#include "Interface/StringizeInterface.h"

#include "Kernel/ScriptEventReceiver.h"

#include "Kernel/NodePrototypeGenerator.h"

#include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ModuleInvader::ModuleInvader()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleInvader::~ModuleInvader()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleInvader::_initialize()
	{
		pybind::kernel_interface * kernel = pybind::get_kernel();

		pybind::interface_<InvaderFollowAffector, pybind::bases<Affector> >( kernel, "InvaderFollowAffector" )
			.def( "setMoveSpeed", &InvaderFollowAffector::setMoveSpeed )
			.def( "getMoveSpeed", &InvaderFollowAffector::getMoveSpeed )
			;

		pybind::def_functor_args( kernel, "createFollowAffector", this, &ModuleInvader::createFollowAffector );
		pybind::def_functor( kernel, "destroyFollowAffector", this, &ModuleInvader::destroyFollowAffector );
		
        m_factoryInvaderFollowAffector = new FactoryPool<InvaderFollowAffector, 4>();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleInvader::_finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
    InvaderFollowAffectorPtr ModuleInvader::createFollowAffector( const NodePtr & _node
		, const NodePtr & _target
		, float _moveSpeed
		, const pybind::object & _cb
		, const pybind::args & _args )
	{
		InvaderFollowAffectorPtr affector = m_factoryInvaderFollowAffector->createObject();

		affector->setMoveSpeed( _moveSpeed );

		if( affector->initialize( _node, _target, _cb, _args ) == false )
		{
			return nullptr;
		}

		if( _node->addAffector( affector ) == INVALID_AFFECTOR_ID )
		{
			return nullptr;
		}

		return affector;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleInvader::destroyFollowAffector( const InvaderFollowAffectorPtr & _affector )
	{
		const NodePtr & node = _affector->getNode();

		uint32_t id = _affector->getId();

		node->stopAffector( id );
	}
}