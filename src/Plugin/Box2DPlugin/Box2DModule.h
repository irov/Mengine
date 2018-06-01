#pragma once

#include "Core/ModuleBase.h"

#include "Box2D/Box2D.h"

#include "Box2DBody.h"

#include "Config/Vector.h"

#include "pybind/pybind.hpp"


namespace Mengine
{
	typedef IntrusivePtr<class Box2DWorld> Box2DWorldPtr;
	
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
		Box2DWorldPtr createWorld( const mt::vec2f& _gravity, const pybind::object & _update, const pybind::args & _update_args );
		void destroyWorld( const Box2DWorldPtr & _world );

	public:
		void _tick( float _time, float _timing ) override;
		void _render( const RenderContext * _state, uint32_t _debugMask ) override;

	protected:
		PyObject * s_Box2DBody_setEventListener( pybind::kernel_interface * _kernel, Box2DBody * _node, PyObject * _args, PyObject * _kwds );

    private:
		typedef Vector<Box2DWorldPtr> TVectorWorlds;
		TVectorWorlds m_worlds;
		TVectorWorlds m_worldsAdd;
    };
}