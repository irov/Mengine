#pragma once

#include "Core/ModuleBase.h"

#include "AreaOfInterest.h"

#include "fastpathfinder/graph.h"

namespace Mengine
{
	class ModuleAreaOfInterest
		: public ModuleBase
	{
	public:
		ModuleAreaOfInterest();
		~ModuleAreaOfInterest() override;

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		AreaOfInterest * createAOI();
		void removeAOI( AreaOfInterest * _aoi );

	public:
		void _tick( float _time, float _timing ) override;
		void _render( const RenderContext * _state, uint32_t _debugMask ) override;
		
	protected:
		typedef Vector<AreaOfInterest *> TVectorAreaOfInterest;
		TVectorAreaOfInterest m_aois;
	};
}