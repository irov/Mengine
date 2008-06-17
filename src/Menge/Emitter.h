#	pragma once

#	include "SceneNode2D.h"

#	include "../Interface/RenderSystemInterface.h"
#	include "../Interface/ParticleSystemInterface.h"

#	include "math/mat3.h"

#	include <vector>

class EmitterInterface;

namespace Menge
{
	class ResourceEmitterContainer;

	class Emitter
		: public SceneNode2D
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
		virtual bool isVisible( const Viewport & _viewPort );

		void loader( XmlElement * _xml ) override;
		void onStopped() override;

	protected:
		void _render() override;
		void _debugRender() override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;

	private:
		ResourceEmitterContainer * m_resource;
		std::string m_resourcename;

		std::string m_emitterName;

		EmitterInterface * m_interface;

		std::vector<RenderImageInterface*> m_images;

		EBlendFactor m_blendSrc;
		EBlendFactor m_blendDest;

		virtual void _onSetListener();

		bool m_autoPlay;
		bool m_looped;
	};
}