#pragma once

#include "HotSpot.h"

#include "ResourceTestPick.h"

#include "Kernel/ResourceHolder.h"

namespace Mengine
{
	class HotSpotImage
		: public HotSpot
	{
	public:
		HotSpotImage();
		~HotSpotImage() override;

	public:
		void setResourceTestPick( const ResourceTestPickPtr & _resourceTestPick );
		const ResourceTestPickPtr & getResourceTestPick() const;

	public:
		void setAlphaTest( float _value );
		float getAlphaTest() const;

        uint32_t getWidth() const;
        uint32_t getHeight() const;

	protected:
		bool testPoint( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const override;
		bool testRadius( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radiusx, float _radiusy ) const override;
		bool testPolygon( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const override;

	protected:
		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;

	protected:
		void _debugRender( const RenderContext * _state ) override;

    protected:
		bool _compile() override;
		void _release() override;

	protected:
        ResourceHolder<ResourceTestPick> m_resourceTestPick;

		float m_alphaTest;		
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HotSpotImage> HotSpotImagePtr;
    //////////////////////////////////////////////////////////////////////////
}	
