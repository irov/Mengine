#pragma once

#include "Kernel/Surface.h"
#include "Kernel/ResourceHolder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ESurfaceTrackMatteMode
    {
        ESTM_MODE_NONE = 0,
        ESTM_MODE_ALPHA = 1,
        ESTM_MODE_ALPHA_INVERTED = 2,
        ESTM_MODE_LUMA = 3,
        ESTM_MODE_LUMA_INVERTED = 4,
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ResourceImage> ResourceImagePtr;
    //////////////////////////////////////////////////////////////////////////
	class SurfaceTrackMatte
		: public Surface
	{
	public:
		SurfaceTrackMatte();
		~SurfaceTrackMatte() override;

	public:
		void setResourceImage( const ResourceImagePtr & _resourceImage );
		const ResourceImagePtr & getResourceImage() const;

	public:
		void setResourceTrackMatteImage( const ResourceImagePtr & _resourceTrackMatteImage );
		const ResourceImagePtr & getResourceTrackMatteImage() const;

    public:
        void setTrackMatteMode( ESurfaceTrackMatteMode _trackMatteMode );
        ESurfaceTrackMatteMode getTrackMatteMode() const;

    public:
        const mt::vec2f & getMaxSize() const override;
        const mt::vec2f & getSize() const override;
        const mt::vec2f & getOffset() const override;

        uint32_t getUVCount() const override;
        const mt::uv4f & getUV( uint32_t _index ) const override;

        const ColourValue & getColor() const override;

        void correctUV( uint32_t _index, mt::vec2f & _out, const mt::vec2f & _in ) override;
	
    protected:
        bool _update( float _current, float _timing ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		RenderMaterialInterfacePtr _updateMaterial() const override;

	protected:
		ResourceHolder<ResourceImage> m_resourceImage;
		ResourceHolder<ResourceImage> m_resourceTrackMatteImage;

        ESurfaceTrackMatteMode m_trackMatteMode;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SurfaceTrackMatte> SurfaceTrackMattePtr;
    //////////////////////////////////////////////////////////////////////////
}
