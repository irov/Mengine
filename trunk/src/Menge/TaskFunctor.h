/*
 *	TaskFunctor.h
 *
 *	Created by _Berserk_ on 27.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Task.h"
#	include "Functor.h"

namespace Menge
{
	class TaskFunctor
		: public Task
	{
	public:
		TaskFunctor( Functor& _functor );
	
	public:
		void main() override;

	protected:
		Functor& m_functor;
	};
}	// namespace Menge
