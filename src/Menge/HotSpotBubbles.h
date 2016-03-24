#	pragma once

#	include "HotSpot.h"

#   include "ResourceHIT.h"

namespace Menge
{
	class HotSpotBubbles
		: public HotSpot
	{
	public:
		HotSpotBubbles();
		~HotSpotBubbles();

	public:
		uint32_t addBubble( const mt::vec2f & _pos, float _radius, float _ellipse, bool _outward );

	protected:
		bool testPoint( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const override;
		bool testRadius( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radius ) const override;
		bool testPolygon( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const override;

	protected:
		void _invalidateWorldMatrix() override;

	protected:
		void invalidatemBubbleWM_();
		void updateBubbleWM_() const;

	protected:
		struct Bubble
		{
			uint32_t id;
			mt::vec2f pos;
			mutable mt::vec2f pos_wm;
			float radius;
			float ellipse;
			bool outward;
		};

		typedef stdex::vector<Bubble> TVectorBubbles;
		TVectorBubbles m_bubbles;

		uint32_t m_enumerator;

		mutable bool m_invalidateBubbleWM;
	};
}	// namespace Menge
