#pragma once

#include "Core/ModuleBase.h"

#include "BurritoWorld.h"

namespace Mengine
{
	class ModuleBurritoWorld
		: public ModuleBase
	{
	public:
		ModuleBurritoWorld();
		~ModuleBurritoWorld() override;

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		BurritoWorld * createBurritoWorld();
		void removeBurritoWorld( BurritoWorld * _world );

	public:
		void _tick( float _time, float _timing ) override;
		void _render( const RenderContext * _state, uint32_t _debugMask ) override;
		
	protected:
		typedef Vector<BurritoWorld *> TVectorBurritoWorld;
		TVectorBurritoWorld m_worlds;
		TVectorBurritoWorld m_worldsAdd;
	};
}