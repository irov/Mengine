#pragma once

#include "Kernel/Surface.h"
#include "Kernel/ResourceHolder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ResourceImage, class Resource> ResourceImagePtr;
    //////////////////////////////////////////////////////////////////////////
	class SurfaceImage
		: public Surface        
	{
	public:
        SurfaceImage();
		~SurfaceImage() override;

	public:
		void setResourceImage( const ResourceImagePtr & _resourceImage );
		const ResourceImagePtr & getResourceImage() const;

    public:
        const mt::vec2f & getMaxSize() const override;
        const mt::vec2f & getSize() const override;
        const mt::vec2f & getOffset() const override;

        uint32_t getUVCount() const override;
        const mt::uv4f & getUV( uint32_t _index ) const override;

        void correctUV( uint32_t _index, mt::vec2f & _out, const mt::vec2f & _in ) override;

        const ColourValue & getColor() const override;

    protected:
		bool _compile() override;
		void _release() override;

    protected:
        bool _update( float _current, float _timing ) override;

	protected:
		RenderMaterialInterfacePtr _updateMaterial() const override;

	protected:
		ResourceHolder<ResourceImage> m_resourceImage;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SurfaceImage> SurfaceImagePtr;
}
