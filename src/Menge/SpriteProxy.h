#	pragma once

#include "Sprite.h"
#include "NodeProxy.h"

namespace Menge
{
	class SpriteProxy 
		: public NodeProxy
	{
	public:
		SpriteProxy(Sprite * _spr);

	public:
		void stop();
		void play();
		void setLooped(bool _looped);
		bool getLooped() const;
		void setFirstFrame();

	public:
		void loader(TiXmlElement *xml);
		void render( const ViewPort & _viewPort );

	private:
		Sprite* m_Sprite;
	};
};