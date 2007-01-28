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

	protected:
		void loader( TiXmlElement * _xml ) override;
		
	private:
		Avatar * m_avatar;
	};
}