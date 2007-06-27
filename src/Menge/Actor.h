#	pragma once

#	include "SceneNode2D.h"

namespace Menge
{
	class Avatar;

	class Actor
		: public SceneNode2D
	{
		OBJECT_DECLARE(Actor)

	public:
		Actor();

	public:
		void moveto(const mt::vec2f& _pos);

	protected:
		void loader( TiXmlElement * _xml ) override;
		void _update( float _timing)  override;

	private:
		bool m_isMove;
		mt::vec2f	m_dir;
		mt::vec2f   m_destpos;
		float m_speedMove;
	};	
}