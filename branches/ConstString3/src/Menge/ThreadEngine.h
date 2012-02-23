/*
 *	ThreadEngine.h
 *
 *	Created by _Berserk_ on 28.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/ThreadSystemInterface.h"
#	include "Core/Holder.h"

namespace Menge
{
	class ThreadEngine
		: public Holder<ThreadEngine>
	{
	public:
		ThreadEngine();
		~ThreadEngine();

	public:
		bool initialize();

	public:
		ThreadSystemInterface* getInterface() const;

	public:
		void sleep( unsigned int _ms );

	protected:
		ThreadSystemInterface* m_threadSystemInterface;
	};
}	// namespace Menge
