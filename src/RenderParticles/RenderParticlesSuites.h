#	pragma once
#	include "RenderParticlesInterface.h"

namespace Menge
{
	class EmitterDescription;
	class RenderParticles;
	class RenderParticlesScriptWrapper;
	class EmitterDescriptionManager;

	class RenderParticlesSuites
		:public RenderParticlesInterface
	{	
	public:
		RenderParticlesSuites();

		bool initialise( HINSTANCE _hinstance, size_t _windowWidth, size_t _windowHeight ) override;
		void destroy() override;
		void addListener( RenderParticlesListener * _listener ) override;
		void removeListener( RenderParticlesListener * _listener ) override;
		void loadAstralaxFile( const String& _path ) override;
		void loadEmitterFromFile( const String& _path, const String & _emitterName, bool _isCached ) override;
		void loadFrame( RenderParticlesFrame * _frame, const TVectorEmitterDescriptionInterface& _emittersToUpdate ) override;
		void log( const String & _message ) override;
		void loop();
	private:
		RenderParticles * m_render;
		RenderParticlesScriptWrapper  * m_scriptWrapper;
		EmitterDescriptionManager * m_emitterDescriptionManager;
	};
}