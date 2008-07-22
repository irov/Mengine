#	pragma once

#	include "Layer2D.h"
#	include "Viewport.h"

namespace Menge
{
	class Camera2D;

	class Layer2DLoop
		: public Layer2D
	{
		OBJECT_DECLARE(Layer2DLoop);

	public:
		Layer2DLoop();

	//public:
		//void setOffsetPosition( const mt::vec2f & _offset ) override;

	//public:
	//	void loader( XmlElement * _xml ) override;
		void update( float _timing ) override;
		void render( const Viewport & _viewport, bool _enableDebug ) override;


	//protected:
		//void _addChildren( Node * _node ) override;
		//bool _activate() override;

	//protected:
		//mt::vec2f m_factorParallax;
	};
}