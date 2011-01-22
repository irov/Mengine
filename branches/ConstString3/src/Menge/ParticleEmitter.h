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
		void setResource( const ConstString& _resourceName );
		void setEmitter( const ConstString& _emitterName );

	public:
		void loader( BinParser * _parser ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;
		void _render( Camera2D * _camera ) override;

		void _setEventListener( PyObject * _listener ) override;
		void _updateBoundingBox( mt::box2f& _boundingBox ) override;

	protected:
		ResourceEmitterContainer * m_resource;
		ConstString m_resourcename;

		ConstString m_emitterName;

		EmitterInterface * m_interface;

		bool m_autoPlay;
		bool m_looped;
		bool m_emitterRelative;

		bool m_centerAlign;

		bool m_onEmitterEndEvent;
		bool m_onEmitterStopEvent;
		float m_startPosition;

		typedef std::vector<const Material*> TVectorMaterial;
		TVectorMaterial m_materials;

		typedef std::vector<TVectorVertex2D> TVectorVertices;
		TVectorVertex2D m_vertices;

		struct Batch
		{
			TVectorVertices::size_type it_begin;
			TVectorVertices::size_type it_end;
			const Texture * texture[1];
			int type;
		};

		typedef std::vector<Batch> TVectorBatchs;
		TVectorBatchs m_batchs;

		typedef std::vector<ResourceImageDefault*> TVectorImages;
		TVectorImages m_images;
		typedef std::vector<std::size_t> TVectorOffsets;
		TVectorOffsets m_imageOffsets;

		Viewport* m_checkViewport;
	};
}
