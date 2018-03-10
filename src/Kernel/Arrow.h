#	pragma once

#	include "Kernel/Entity.h"

#	include "Core/Resolution.h"

#	include "Math/vec4.h"

#	include "Core/Polygon.h"

namespace Menge
{
	enum EArrowType
	{
		EAT_POINT,
		EAT_RADIUS,
		EAT_POLYGON
	};

	class Arrow
		: public Entity		
	{
	public:
		Arrow();

	public:
		void setHide( bool _value ) override;

	public:
		EArrowType getArrowType() const;

	public:
		void setOffsetClick( const mt::vec2f & _offsetClick );
		const mt::vec2f & getOffsetClick() const;

		void setPolygon( const Polygon & _polygon );
		const Polygon & getPolygon() const;

		void setRadius( float _radius );
		float getRadius() const;

	public:
		void calcMouseWorldPosition( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint ) const;		
		void calcPointClick( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint ) const;		
		void calcPointDeltha( const RenderCameraInterface * _camera, const mt::vec2f & _screenPoint, const mt::vec2f & _screenDeltha, mt::vec2f & _worldDeltha ) const;

	public:
		void calcMouseScreenPosition( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const mt::vec2f & _worldPoint, mt::vec2f & _screenPoint ) const;
		
	public:
		void adaptScreenPosition_( const mt::vec2f & _screenPoint, mt::vec2f & _adaptScreenPoint ) const;
		void adaptScreenDeltha_( const mt::vec2f & _screenDeltha, mt::vec2f & _adaptScreenDeltha ) const;
		void adaptWorldPosition_( const mt::vec2f & _screenPoint, mt::vec2f & _adaptScreenPoint ) const;

	public:
		void onAppMouseLeave();
		void onAppMouseEnter();

	protected:
		bool _activate() override;
		void _deactivate() override;

    protected:
		void _debugRender( RenderServiceInterface * _renderService, const RenderObjectState * _state, uint32_t _debugMask ) override;
        
	protected:
		EArrowType m_arrowType;

        uint32_t m_inputMousePositionProviderId;

		mt::vec2f m_pointClick;
		float m_radius;
		Polygon m_polygon;
		mutable Polygon m_polygonScreen;

		bool m_hided;

        class ArrowInputMousePositionProvider;
	};
}
