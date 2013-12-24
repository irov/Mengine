#	pragma once

#   include "Interface/StreamInterface.h"
#   include "Interface/VideoCodecInterface.h"

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"
#	include "Kernel/FixedVertices.h"

#   include "ResourceVideo.h"

namespace Menge
{
    class FileServiceInterface;
    class CodecServiceInterface;
    class ResourceServiceInterface;
    class NodeServiceInterface;
    class RenderServiceInterface;

	class RenderTextureInterface;
	
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
		void setResourceVideo( ResourceVideo * _resourceVideo );
		ResourceVideo * getResourceVideo() const;

		void _setTiming( float _timing ) override;
		float _getTiming() const override;

		void _setFirstFrame() override;
		void _setLastFrame() override;

		void setBlendAdd( bool _blendAdd );
		bool isBlendAdd() const;

	protected:
		void _update( float _current, float _timing ) override;
		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;
		
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
		void _updateVertices( RenderVertex2D * _vertices, unsigned char _invalidateVertices ) override;

	protected:
		void invalidateMaterial_();
		void updateMaterial_();

		inline const RenderMaterialInterfacePtr & getMaterial();

	protected:
		ResourceHolder<ResourceVideo> m_resourceVideo;

		RenderTextureInterfacePtr m_textures[1];
		
		RenderMaterialInterfacePtr m_material;
		bool m_invalidateMaterial;

		VideoDecoderInterfacePtr m_videoDecoder;
		mt::vec2f m_frameSize;
		mt::vec4f m_uv;

		float m_timing;        

		bool m_blendAdd;
		bool m_autoStart;
		bool m_needUpdate;
        bool m_needUpdate2;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const RenderMaterialInterfacePtr & Video::getMaterial()
	{
		if( m_invalidateMaterial == true )
		{
			this->updateMaterial_(); 
		}

		return m_material;
	}
}
