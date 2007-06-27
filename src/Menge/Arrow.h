#	pragma once

#	include "SceneNode2D.h"

namespace Menge
{
	class Sprite;

	class Arrow
		: public SceneNode2D
	{
		OBJECT_DECLARE(Arrow)

	public:
		Arrow();

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _update( float _timing ) override;
		bool _activate() override;
		void _debugRender() override;

	private:
		Sprite * m_arrowIdle;
		Sprite * m_arrowClick;

		mt::vec2f m_offsetClick;
	};
}