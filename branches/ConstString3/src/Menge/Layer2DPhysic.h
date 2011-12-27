#	pragma once

#	include "Layer2D.h"


namespace Menge
{
	class Camera2D;

	class Layer2DPhysic
		: public Layer2D
	{
	public:
		Layer2DPhysic();

	protected:
		void loader( BinParser * _parser ) override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;
		void _initialize();
		void _removeChildren( Node * _node ) override;
	};
}
