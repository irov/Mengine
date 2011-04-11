//#	pragma once
//
//#	include "Layer2D.h"
//#	include "Core/Viewport.h"
//
//namespace Menge
//{
//	class Camera2D;
//
//	class Layer2DLoop
//		: public Layer2D
//	{
//	public:
//		Layer2DLoop();
//
//	public:
//		void update( float _timing ) override;
//		void render( Camera2D * _camera ) override;
//
//		bool testBoundingBox( const Viewport & _viewport, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const override;
//		bool testArrow( const Viewport & _viewport, HotSpot * _layerspaceHotspot, Arrow * _arrow ) const override;
//		bool testPoint( const Viewport & _viewport, HotSpot * _layerspaceHotspot, const mt::vec2f& _point ) const override;
//
//		mt::vec2f calcScreenPosition( const Viewport& _viewport, Node* _node ) const override;
//
//	protected:
//		bool _activate() override;
//		void _deactivate() override;
//
//	protected:
//		Camera2D* m_camera2DLeft;
//		Camera2D* m_camera2DRight;
//	};
//}
