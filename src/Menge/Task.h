/*
 *	Task.h
 *
 *	Created by _Berserk_ on 24.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Config/Typedef.h"
#	include "Interface/ThreadSystemInterface.h"

namespace Menge
{
	class Task
		: public ThreadInterface
	{
	public:
		Task();
		virtual ~Task();

	public:
		void destroy();

	public:
		bool isComplete() const override;
		
	public:
		virtual void preMain();
		virtual void postMain();
		virtual void update();
		virtual void cancel();
		virtual void cleanup();

	protected:
		bool m_complete;
	};
}	// namespace Menge
