#	pragma once

#	include "Core/ModuleBase.h"

#	include "AreaOfInterest.h"

#	include "fastpathfinder/graph.h"

namespace Menge
{
	class ModuleAreaOfInterest
		: public ModuleBase
	{
	public:
		ModuleAreaOfInterest();
		~ModuleAreaOfInterest();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		AreaOfInterest * createAOI();
		void removeAOI( AreaOfInterest * _aoi );

	public:
		void _update( float _time, float _timing ) override;
		void _render( const RenderObjectState * _state, unsigned int _debugMask ) override;
		
	protected:
		typedef stdex::vector<AreaOfInterest *> TVectorAreaOfInterest;
		TVectorAreaOfInterest m_aois;
	};
}