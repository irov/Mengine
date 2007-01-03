#	pragma once

#	include "Allocator.h"

namespace Menge
{
	class Avatar;

	class Actor
		: public Allocator
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