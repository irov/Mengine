#pragma once

#include "Core/ModuleBase.h"
#include "Factory/Factory.h"

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
		InvaderFollowAffectorPtr createFollowAffector( const NodePtr & _node
			, const NodePtr & _target
			, float _moveSpeed
			, const pybind::object & _cb
			, const pybind::args & _args );

		void destroyFollowAffector( const InvaderFollowAffectorPtr & _affector );

	protected:
		FactoryPtr m_factoryInvaderFollowAffector;
	};
}