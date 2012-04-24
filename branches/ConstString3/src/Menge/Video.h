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

	/*! xml - файл имеет следующую структуру:
	*	<Node Name = "имя_ноды" Type = "Video">
	*		<ImageMap Name = "имя_ресурса_видео"/>
	*		<AutoStart Value = "1/0"/>
	*	</Node>
	*/

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

	protected:
		void _update( float _timing ) override;
		void _render( Camera2D * _camera ) override;
		
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _setEventListener( PyObject * _listener ) override;
		void _invalidateWorldMatrix() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _invalidateColor() override;
		
		bool _sync( float _timing );
		void _rewind();
		bool _compileDecoder();
        
		virtual bool _play();
		virtual bool _restart( size_t _enumerator );
		virtual void _stop( size_t _enumerator );
		virtual void _end( size_t _enumerator );
	
	protected:
		void _updateVertices( Vertex2D * _vertices, unsigned char _invalidateVertices ) override;
	protected:
		ResourceVideo* m_resourceVideo;

		ConstString m_resourceVideoName;
		ConstString m_resourceSoundName;

		const RenderTextureInterface * m_textures[1];
		SoundEmitter* m_soundEmitter;

		const RenderMaterialGroup * m_materialGroup;
		const RenderMaterial * m_material;	

		FileInputStreamInterface * m_videoFile;
		VideoDecoderInterface * m_videoDecoder;
		mt::vec2f m_frameSize;

		bool m_autoStart;
		bool m_loop;
		float m_timing;
		bool m_needUpdate;
	};
}
