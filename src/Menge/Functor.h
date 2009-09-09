/*
 *	Functor.h
 *
 *	Created by _Berserk_ on 27.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

namespace Menge
{
	class Functor
	{
	public:
		virtual void operator()() = 0;
	};

	template< typename ClassType >
	class FunctorMember
		: public Functor
	{
	public:
		typedef void (ClassType::*TMemberFunc)();
		FunctorMember( ClassType* _object, TMemberFunc _member )
			: m_object( _object )
			, m_member( _member )
		{
		}

	public:
		void operator()()
		{
			m_object->m_member();
		}

	private:
		ClassType* m_object;
		TMemberFunc m_member;
	};

}	// namespace Menge
