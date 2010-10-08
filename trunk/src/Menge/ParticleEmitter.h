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
	class Texture;

	class ParticleEmitter
		: public Node
		, public ParticleEmitterListenerInterface
	{
	public:
		ParticleEmitter();
		~ParticleEmitter();

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
	
		void onStopped() override;
		void setResource( const String& _resourceName );
		void setEmitter( const String& _emitterName );

		void setStartPosition( float _pos );

	public:
		void loader( XmlElement * _xml ) override;
		void parser( BinParser * _parser ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;
		void _render( Camera2D * _camera ) override;

		void _setListener( PyObject * _listener ) override;
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
		TVertex2DVector m_vertices;

		struct Batch
		{
			TVectorVertices::size_type it_begin;
			TVectorVertices::size_type it_end;
			Texture * texture;
			int type;
		};

		typedef std::vector<Batch> TVectorBatchs;
		TVectorBatchs m_batchs;

		typedef std::vector<ResourceImageDefault*> TVectorImages;
		TVectorImages m_images;
		typedef std::vector<std::size_t> TVectorOffsets;
		TVectorOffsets m_imageOffsets;

		Viewport* m_checkViewport;
		bool m_playing;
	};
}
