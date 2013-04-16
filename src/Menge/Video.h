#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"
#	include "Kernel/FixedVertices.h"

namespace Menge
{
    class FileServiceInterface;
    class CodecServiceInterface;
    class ResourceServiceInterface;
    class NodeServiceInterface;
    class RenderServiceInterface;

	class ResourceVideo;
	//class ResourceSound;
	class SoundEmitter;
	class SoundSourceInterface;
	class RenderTextureInterface;
	
	class InputStreamInterface;
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
		
    protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _setEventListener( PyObject * _listener ) override;
		void _invalidateWorldMatrix() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _invalidateColor() override;
		
    protected:
		void updateUV_();
		void updateMaterial_();

		bool sync_( float _timing );
		bool compileDecoder_();
        bool fillVideoBuffer_();

	protected:
		bool _play( float _time ) override;
		bool _restart( float _time, size_t _enumerator ) override;
		void _stop( size_t _enumerator ) override;
		void _end( size_t _enumerator ) override;
		bool _interrupt( size_t _enumerator ) override;

	protected:
		void _updateVertices( Vertex2D * _vertices, unsigned char _invalidateVertices ) override;

	protected:
		ResourceVideo * m_resourceVideo;

		ConstString m_resourceVideoName;
		ConstString m_resourceSoundName;

		RenderTextureInterface * m_textures[1];
		SoundEmitter* m_soundEmitter;
		
		const RenderMaterialGroup * m_materialGroup;
		const RenderMaterial * m_material;	

		VideoDecoderInterface * m_videoDecoder;
		mt::vec2f m_frameSize;
		mt::vec4f m_uv;

		float m_timing;
        size_t m_playIterator;

		bool m_blendAdd;
		bool m_autoStart;
		bool m_needUpdate;
        bool m_needUpdate2;
	};
}
