#	pragma once

#	include "Core/ModuleBase.h"

#	include "BurritoWorld.h"

namespace Menge
{
	class ModuleBurritoWorld
		: public ModuleBase
	{
	public:
		ModuleBurritoWorld();
		~ModuleBurritoWorld();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		BurritoWorld * createBurritoWorld();
		void removeBurritoWorld( BurritoWorld * _world );

	public:
		void _update( float _time, float _timing ) override;
		void _render( const RenderObjectState * _state, unsigned int _debugMask ) override;
		
	protected:
		typedef stdex::vector<BurritoWorld *> TVectorBurritoWorld;
		TVectorBurritoWorld m_worlds;
		TVectorBurritoWorld m_worldsAdd;
	};
}