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
		void nextFrame();
		void prevFrame();
		void setOffset(const mt::vec2f& _offset);
	public:
		bool isVisible(const Viewport & _viewPort);
	public:
		void loader(TiXmlElement *xml);
		bool render( const Viewport & _viewPort );

	private:
		Sprite* m_Sprite;
	};
};