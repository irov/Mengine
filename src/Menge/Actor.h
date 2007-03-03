#	pragma once

#	include "Allocator2D.h"

namespace Menge
{
	class Avatar;

	class Actor
		: public Allocator2D
	{
		OBJECT_DECLARE(Actor)

	public:
		Actor();

		void moveto(const mt::vec2f& _pos);


	protected:
		void loader( TiXmlElement * _xml ) override;
		void _update( float _timing)  override;
	private:
		Avatar * m_avatar;
		bool m_ismove;
		mt::vec2f	m_dir;
		mt::vec2f   m_destpos;
		float m_speed;
	};
}