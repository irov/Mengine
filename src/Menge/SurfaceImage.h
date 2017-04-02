#	pragma once

#	include "Kernel/Surface.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class ResourceImage> ResourceImagePtr;
    //////////////////////////////////////////////////////////////////////////
	class SurfaceImage
		: public Surface        
	{
	public:
        SurfaceImage();
		~SurfaceImage();

	public:
		void setResourceImage( const ResourceImagePtr & _resourceImage );
		const ResourceImagePtr & getResourceImage() const;

    public:
        const mt::vec2f & getMaxSize() const override;
        const mt::vec2f & getSize() const override;
        const mt::vec2f & getOffset() const override;

        uint32_t getUVCount() const override;
        const mt::uv4f & getUV( uint32_t _index ) const override;

        const ColourValue & getColour() const override;

	protected:
		bool update( float _current, float _timing ) override;

    protected:
		bool _compile() override;
		void _release() override;

	protected:
		RenderMaterialInterfacePtr _updateMaterial() const override;

	protected:
		ResourceHolder<ResourceImage> m_resourceImage;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<SurfaceImage> SurfaceImagePtr;
}
