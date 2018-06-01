#pragma once

#include "Core/ModuleBase.h"

#include "MagnetWorld.h"
#include "MagnetActor.h"
#include "NodeMagnetActor.h"

#include "Factory/Factory.h"

namespace Mengine
{
	class ModuleMagnet
		: public ModuleBase
	{
	public:
		ModuleMagnet();
		~ModuleMagnet() override;

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		MagnetWorldPtr createMagnetWorld();
		void removeMagnetWorld( const MagnetWorldPtr & _collision );

	public:
		void _tick( float _time, float _timing ) override;
		void _render( const RenderContext * _state, uint32_t _debugMask ) override;
		
	protected:
		FactoryPtr m_factoryMagnetWorld;

		typedef Vector<MagnetWorldPtr> TVectorMagnetWorlds;
		TVectorMagnetWorlds m_magnetWorlds;
	};
}