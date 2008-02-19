#	pragma once

#	include "Layer.h"
#	include "SceneNode3D.h"

namespace Menge
{
	class Layer3D
		: public Layer
		, public SceneNode3D
	{
		OBJECT_DECLARE(Layer3D);

	public:
		Layer3D();

	public:
		void loader( XmlElement * _xml ) override;

		bool handleKeyEvent( size_t _key, size_t _char, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( int _x, int _y, int _whell ) override;

	public:
		void update( float _timing ) override;
	
	protected:

		bool _activate() override;
		void _addChildren( Node * _node ) override;
	};
}