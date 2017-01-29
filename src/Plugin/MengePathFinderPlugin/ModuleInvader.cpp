#	include "ModuleInvader.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/PlayerInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "PythonScriptWrapper/ScriptClassWrapper.h"

#	include "Kernel/NodePrototypeGenerator.h"

#	include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
namespace Menge
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
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleInvader::_finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	InvaderFollowAffector * ModuleInvader::createFollowAffector( Node * _node
		, Node * _target
		, float _moveSpeed
		, const pybind::object & _cb
		, const pybind::detail::args_operator_t & _args )
	{
		InvaderFollowAffector * affector = m_factoryInvaderFollowAffector.createObject();

		affector->setMoveSpeed( _moveSpeed );

		if( affector->initialize( _node, _target, _cb, _args ) == false )
		{
			affector->destroy();

			return nullptr;
		}

		if( _node->addAffector( affector ) == INVALID_AFFECTOR_ID )
		{
			affector->destroy();

			return nullptr;
		}

		return affector;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleInvader::destroyFollowAffector( InvaderFollowAffector * _affector )
	{
		Node * node = _affector->getNode();

		uint32_t id = _affector->getId();

		node->stopAffector( id );
	}
}