#	pragma once

#	include "Node.h"

#	include "../Interface/RenderSystemInterface.h"
#	include "../Interface/ParticleSystemInterface.h"

#	include "math/mat3.h"

#	include <vector>

class EmitterInterface;

namespace Menge
{
	class ResourceEmitterContainer;

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
		void setLooped( int _loop );
		int	 getLooped() const;
		void setLeftBorder( float _leftBorder );
	
	public:
		void loader( XmlElement * _xml ) override;
		void onStopped() override;

	protected:
		void _render( const Viewport & _viewport, bool _enableDebug ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;

		void _setListener() override;

	private:
		ResourceEmitterContainer * m_resource;
		std::string m_resourcename;

		std::string m_emitterName;

		EmitterInterface * m_interface;

		std::vector<RenderImageInterface*> m_images;

		EBlendFactor m_blendSrc;
		EBlendFactor m_blendDest;

		//virtual void _onSetListener();

		bool m_autoPlay;
		bool m_looped;
	};
}