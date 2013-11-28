#	pragma once

#	include "Kernel/Entity.h"

#	include "Interface/InputSystemInterface.h"

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
		, public InputMousePositionProvider
	{
	public:
		Arrow();

	public:
		void hide( bool _value ) override;

	public:
		Scene * getScene() override;

	public:
		EArrowType getArrowType() const;

	public:
		void setPointClick( const mt::vec2f & _offsetClick );
		const mt::vec2f & getPointClick() const;

		void setPolygon( const Polygon & _polygon );
		const Polygon & getPolygon() const;

		void setRadius( float _radius );
		float getRadius() const;		
		
	public:
		void onAppMouseLeave();
		void onAppMouseEnter();

	protected:
		bool _activate() override;
		void _deactivate() override;

    protected:
        void _debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) override;

	protected:
		void onMousePositionChange( const mt::vec2f & _position ) override;

	protected:
		EArrowType m_arrowType;

		mt::vec2f m_pointClick;
		float m_radius;
		Polygon m_polygon;

		bool m_hided;
	};
}
