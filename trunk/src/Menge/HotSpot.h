#	pragma once

#	include "Node.h"

#	include "InputHandler.h"
#	include "MousePickerTrap.h"

#	include "Math/polygon.h"
#	include "Node.h"
#	include "Vertex.h"

namespace Menge
{
	struct Material;

	class HotSpot
		: public Node
		, public GlobalMouseHandler
		, public GlobalKeyHandler
		, public MousePickerTrap
	{
		FACTORABLE_DECLARE(HotSpot)
	
	public:
		HotSpot();
		virtual ~HotSpot();

	public:
		const mt::polygon & getPolygon() const;

	protected:
		bool _pickerActive() const override;

	public:
		bool pick( HotSpot * _hotspot ) override;
		void onLeave() override;
		void onEnter() override;

		virtual bool testPolygon( const mt::mat3f& _transform, const mt::polygon& _screenPoly, const mt::mat3f& _screenTransform );

	public:
		void addPoint( const mt::vec2f & _p );
		bool testPoint( const mt::vec2f & _p );
		void clearPoints();

		void enableGlobalMouseEvent( bool _value );
		void enableGlobalKeyEvent( bool _value );
		
	public:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;

	public:
		bool handleGlobalMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleGlobalMouseMove( float _x, float _y, int _whell ) override;
		bool handleGlobalKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;

	public:
		virtual void loader( XmlElement *_xml) override;

	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;

		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _setListener() override;

	protected:
		mt::polygon m_polygon;

		bool m_globalMouseEventListener;
		bool m_globalKeyEventListener;

		bool m_onLeaveEvent;
		bool m_onEnterEvent;

		std::size_t m_pickerId;

#	ifndef MENGE_MASTER_RELEASE
	protected:
		void _render( Camera2D * _camera ) override;
		void _invalidateWorldMatrix() override;

	private:
		void updateVertices_();

	protected:
		uint32 m_debugColor;
		bool m_debugInvalidateVertices;
		TVertex2DVector m_debugVertices;
#	endif
	};
}
