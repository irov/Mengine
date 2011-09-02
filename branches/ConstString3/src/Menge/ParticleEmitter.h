#	pragma once

#	include "Node.h"
#	include "Animatable.h"

#	include "../Interface/RenderSystemInterface.h"
#	include "../Interface/ParticleSystemInterface.h"

#	include "Math/mat3.h"

#	include "Core/Polygon.h"

#	include <vector>

namespace Menge
{
	class ResourceEmitterContainer;
	class EmitterInterface;
	class ResourceImage;
	class ResourceImageDefault;
	struct Material;
	class Texture;

	class ParticleEmitter
		: public Node
		, public ParticleEmitterListenerInterface
		, public Animatable
	{
	public:
		ParticleEmitter();
		~ParticleEmitter();

	public:
		bool _play() override;
		void _stop( std::size_t _enumerator ) override;
		void _end( std::size_t _enumerator ) override;

	public:
		void pause();
		void restart();

		void setLoop( bool _value );
		bool getLoop() const;

		void interrupt();
				
		void setLeftBorder( float _leftBorder );
		void playFromPosition( float _pos );

		void setEmitterTranslateWithParticle( bool _with );
		void setEmitterRelative( bool _relative );
		void setStartPosition( float _pos );

		void setEmitterImage( const ConstString & _emitterImageName );

		void changeEmitterPolygon( const Polygon & _polygon );
	
		void onParticleEmitterStopped() override;
		void setResource( const ConstString& _resourceName );
		void setEmitter( const ConstString& _emitterName ); 

	public:
		void loader( BinParser * _parser ) override;

	protected:
		void _setEventListener( PyObject * _listener );

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;
		void _render( Camera2D * _camera ) override;

		void _updateBoundingBox( mt::box2f& _boundingBox ) override;
		void _invalidateWorldMatrix() override;

	private:
		bool compileImage_();
		bool compileEmitterImage_();

		void updateRelative_();

	protected:
		ResourceEmitterContainer * m_resource;
		ConstString m_resourcename;

		ConstString m_emitterName;

		ConstString m_emitterImageName;

		EmitterInterface * m_interface;

		bool m_emitterRelative;

		bool m_cacheEmitterRelative;

		bool m_centerAlign;

		float m_startPosition;

		const Material * m_materials[2]; //intensive and non intensive

		typedef std::vector<TVectorVertex2D> TVectorVertices;
		TVectorVertex2D m_vertices;

		struct Batch
		{
			TVectorVertices::size_type begin;
			TVectorVertices::size_type size;
			const Texture * texture[1];
			const Material * material;
		};

		typedef std::vector<Batch> TVectorBatchs;
		TVectorBatchs m_batchs;

		Viewport* m_checkViewport;
	};
}
