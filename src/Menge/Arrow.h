#	pragma once

#	include "Renderable.h"

namespace Menge
{
	class HotSpot;
	class Sprite;

	class Arrow
		: public Renderable
	{
		OBJECT_DECLARE(Arrow)

	public:
		HotSpot *pickHotSpot();

	public:
		void loader( TiXmlElement *_xml ) override;

	protected:
		void _update( float _timing ) override;
		bool _compile() override;
		void _debugRender() override;

	private:
		Sprite *m_arrowIdle;
		Sprite *m_arrowClick;
	};

}