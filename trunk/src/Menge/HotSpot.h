#	pragma once

#	include "Node.h"

#	include "InputHandler.h"
#	include "MousePickerTrap.h"
#	include "VectorVertices.h"

#	include "Math/polygon.h"
#	include "Node.h"
#	include "Vertex.h"

namespace Menge
{
	class Arrow;

	class HotSpot
		: public Node
		, public GlobalMouseHandler
		, public GlobalKeyHandler
		, public MousePickerTrap
#	ifndef MENGE_MASTER_RELEASE
		, public VectorVertices
#	endif
	{
		FACTORABLE_DECLARE(HotSpot)
	
	public:
		HotSpot();
		~HotSpot();

	public:
		const mt::polygon & getPolygon() const;

	protected:
		bool _pickerActive() const override;

	public:
		bool pick( Arrow * _arrow ) override;
		
		bool onEnter() override;
		void onLeave() override;
		

		virtual bool testPolygon( const mt::mat3f& _transform, const mt::polygon& _screenPoly, const mt::mat3f& _screenTransform );

	public:
		void addPoint( const mt::vec2f & _p );
		bool testPoint( const mt::vec2f & _p );
		void clearPoints();

		void enableGlobalMouseEvent( bool _value );
		void enableGlobalKeyEvent( bool _value );

	protected:
		void applyGlobalMouseEvent_( bool _value );
		void applyGlobalKeyEvent_( bool _value );
		
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

		void _enable() override;
		void _disable() override;

		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _setListener() override;

		void _changeParent( Node * _parent ) override;

	protected:
		mt::polygon m_polygon;

		bool m_globalMouseEventListener;
		bool m_globalKeyEventListener;

		bool m_onLeaveEvent;
		bool m_onEnterEvent;

		std::size_t m_pickerId;

#	ifndef MENGE_MASTER_RELEASE
	protected:
		void _debugRender( Camera2D * _camera, unsigned int _debugMask ) override;
		void _invalidateWorldMatrix() override;

	private:
		void _updateVertices( VectorVertices::TVectorVertex2D & _vertices, unsigned char _invalidate ) override;

	protected:
		uint32 m_debugColor;
#	endif
	};
}
