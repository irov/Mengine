#	pragma once

#	include "Node.h"
#	include "Animatable.h"
#	include "FixedVertices.h"

namespace Menge
{
	class ResourceVideo;
	//class ResourceSound;
	class SoundEmitter;
	class SoundSourceInterface;
	class RenderTextureInterface;
	
	class FileInputStreamInterface;
	class VideoDecoderInterface;
	
	struct RenderMaterialGroup;

	class Video
		: public Node
		, public QuadVertices
		, public Animatable
	{
	public:
		Video();

	public:
		virtual void pause();
		
	public:
		void setVideoResource( const ConstString& _resource );
		const ConstString & getVideoResource() const;

		void setSoundResource( const ConstString& _resource );
		const ConstString & getSoundResource() const;
		
		void _setTiming( float _timing ) override;
		float _getTiming() const override;

		void _setFirstFrame() override;
		void _setLastFrame() override;

		void setBlendAdd( bool _blendAdd );
		bool isBlendAdd() const;

	protected:
		void _update( float _current, float _timing ) override;
		void _render( RenderCameraInterface * _camera ) override;
		
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _setEventListener( PyObject * _listener ) override;
		void _invalidateWorldMatrix() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _invalidateColor() override;
		
		void updateUV_();
		void updateMaterial_();

		bool sync_( float _timing );
		bool _compileDecoder();
        bool _fillVideoBuffer();

	protected:
		bool _play() override;
		bool _restart( size_t _enumerator ) override;
		void _stop( size_t _enumerator ) override;
		void _end( size_t _enumerator ) override;
		bool _interrupt( size_t _enumerator ) override;

	protected:
		void _updateVertices( Vertex2D * _vertices, unsigned char _invalidateVertices ) override;
		void _seekForce( float _timing );

	protected:
		ResourceVideo* m_resourceVideo;

		ConstString m_resourceVideoName;
		ConstString m_resourceSoundName;

		RenderTextureInterface * m_textures[1];
		SoundEmitter* m_soundEmitter;
		
		const RenderMaterialGroup * m_materialGroup;
		const RenderMaterial * m_material;	

		FileInputStreamInterface * m_videoFile;
		VideoDecoderInterface * m_videoDecoder;
		mt::vec2f m_frameSize;
		mt::vec4f m_uv;

		float m_timing;
        size_t m_playIterator;

		bool m_blendAdd;
		bool m_autoStart;
		bool m_needUpdate;
	};
}
