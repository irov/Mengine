#pragma once

#include "Core/ModuleBase.h"

#include "Box2D/Box2D.h"

#include "Box2DBody.h"

#include "pybind/pybind.hpp"

#include "stdex/stl_vector.h"

namespace Mengine
{
	class Box2DWorld;
	
	class Box2DModule
		: public ModuleBase
        , public b2ContactListener
    {
    public:
		Box2DModule();
		~Box2DModule() override;

	public:
		bool _initialize() override;
		void _finalize() override;

    public:
		Box2DWorld * createWorld( const mt::vec2f& _gravity, const pybind::object & _update, const pybind::args & _update_args );
		void destroyWorld( Box2DWorld * _world );

	public:
		void _tick( float _time, float _timing ) override;
		void _render( const RenderState * _state, uint32_t _debugMask ) override;

	protected:
		PyObject * s_Box2DBody_setEventListener( pybind::kernel_interface * _kernel, Box2DBody * _node, PyObject * _args, PyObject * _kwds );

    private:
		typedef stdex::vector<Box2DWorld *> TVectorWorlds;
		TVectorWorlds m_worlds;
		TVectorWorlds m_worldsAdd;
    };
}