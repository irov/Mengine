#include "Sprite.h"
#include "NodeProxy.h"

namespace Menge
{
	class SpriteProxy 
		: public NodeProxy
	{
	public:
		SpriteProxy(Sprite * _spr);
		void stop();
		void play();
		void setLooped(bool _looped);
		bool getLooped() const;
		void setFirstFrame();
	public:
		void loader(TiXmlElement *xml);
		void render();
	protected:
		void _update(float _timing) override;
		bool _compile() override;
		void _release() override;
	private:
		Sprite* m_Sprite;
	};
};