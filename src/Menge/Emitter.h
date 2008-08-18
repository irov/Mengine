#	pragma once

#	include "Node.h"

#	include "../Interface/RenderSystemInterface.h"
#	include "../Interface/ParticleSystemInterface.h"

#	include "math/mat3.h"

#	include <vector>

namespace Menge
{
	class ResourceEmitterContainer;
	class EmitterInterface;

	class Emitter
		: public Node
		, public ParticleEmitterListenerInterface
	{
		OBJECT_DECLARE(Emitter);
	public:
		Emitter();
		~Emitter();

	public:
		void play();
		void pause();
		void stop();
		void restart();
		void setLooped( int _loop );
		int	 getLooped() const;
		void setLeftBorder( float _leftBorder );
	
	public:
		void loader( XmlElement * _xml ) override;
		void onStopped() override;
		void setResource( const String& _resourceName );
		void setEmitter( const String& _emitterName );

		void _render( unsigned int _debugMask ) override;

	protected:
		bool _checkVisibility( const Viewport & _viewPort ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;

		void _setListener() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;

	private:
		ResourceEmitterContainer * m_resource;
		String m_resourcename;

		String m_emitterName;

		EmitterInterface * m_interface;

		std::vector<RenderImageInterface*> m_images;

		EBlendFactor m_blendSrc;
		EBlendFactor m_blendDest;

		//virtual void _onSetListener();

		bool m_autoPlay;
		bool m_looped;

		bool m_onEmitterEndEvent;
		bool m_onEmitterStopEvent;
		float m_startPosition;
	};
}