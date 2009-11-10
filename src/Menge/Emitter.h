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
	class ResourceImageDefault;
	struct Material;

	class Emitter
		: public Node
		, public ParticleEmitterListenerInterface
	{
		FACTORABLE_DECLARE(Emitter);
	public:
		Emitter();
		~Emitter();

	public:
		void play();
		void pause();
		void stop();
		void restart();
		
		void setLooped( bool _loop );
		bool getLooped() const;

		void setAutoPlay( bool _autoPlay );
		bool getAutoPlay() const;

		void setLeftBorder( float _leftBorder );
		void playFromPosition( float _pos );
		void setEmitterRelative( bool _relative );
	
	public:
		void loader( XmlElement * _xml ) override;
		void onStopped() override;
		void setResource( const String& _resourceName );
		void setEmitter( const String& _emitterName );

		void _render( Camera2D * _camera ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;

		void _setListener() override;
		void _updateBoundingBox( mt::box2f& _boundingBox ) override;

	protected:
		ResourceEmitterContainer * m_resource;
		String m_resourcename;

		String m_emitterName;

		EmitterInterface * m_interface;

		bool m_autoPlay;
		bool m_looped;
		bool m_emitterRelative;

		bool m_onEmitterEndEvent;
		bool m_onEmitterStopEvent;
		float m_startPosition;

		EBlendFactor m_blendSrc;
		EBlendFactor m_blendDst;

		typedef std::vector<Material*> TMaterialVector;
		TMaterialVector m_materials;

		typedef std::vector<TVertex2DVector> TVectorVertices;
		TVectorVertices m_vertices;

		typedef std::vector<ResourceImageDefault*> TVectorTextures;
		TVectorTextures m_images;
		typedef std::vector<std::size_t> TVectorOffsets;
		TVectorOffsets m_imageOffsets;

		TVectorRenderParticle m_particles;
		TVectorOffsets m_particleOffsets;

		Viewport* m_checkViewport;
	};
}
