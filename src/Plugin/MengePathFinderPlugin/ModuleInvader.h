#	pragma once

#	include "Core/ModuleBase.h"

#	include "InvaderFollowAffector.h"

namespace Menge
{
	class ModuleInvader
		: public ModuleBase
	{
	public:
		ModuleInvader();
		~ModuleInvader();
			
	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		InvaderFollowAffector * createFollowAffector( Node * _node
			, Node * _target
			, float _moveSpeed
			, const pybind::object & _cb
			, const pybind::detail::args_operator_t & _args );

		void destroyFollowAffector( InvaderFollowAffector * _affector );

	protected:
		FactoryPtr m_factoryInvaderFollowAffector;
	};
}