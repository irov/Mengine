#pragma once

#include "Core/ModuleBase.h"

#include "InvaderFollowAffector.h"

namespace Mengine
{
	class ModuleInvader
		: public ModuleBase
	{
	public:
		ModuleInvader();
		~ModuleInvader() override;
			
	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		InvaderFollowAffector * createFollowAffector( Node * _node
			, Node * _target
			, float _moveSpeed
			, const pybind::object & _cb
			, const pybind::args & _args );

		void destroyFollowAffector( InvaderFollowAffector * _affector );

	protected:
		FactoryPtr m_factoryInvaderFollowAffector;
	};
}