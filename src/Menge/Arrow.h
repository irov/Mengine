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
		void setOffsetClick( const mt::vec2f & _offsetClick );
		const mt::vec2f & getOffsetClick();

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _renderBegin() override;
		void _renderEnd() override;

	protected:
		void _update( float _timing ) override;
		bool _activate() override;
		void _debugRender() override;

	protected:
		mt::vec2f m_offsetClick;
	};
}