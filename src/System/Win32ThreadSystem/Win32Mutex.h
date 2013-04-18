/*
 *	PosixMutex.h
 *
 *	Created by _Berserk_ on 25.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include <pthread.h>

namespace Menge
{
	class PosixMutex
		: public MutexInterface
	{
	public:
		PosixMutex();
		~PosixMutex();

		bool initialize();
	public:
		void lock() override;
		void unlock() override;
		bool isLocked() const override;

	private:
		pthread_mutex_t m_pmid;
		bool m_locked;
	};
}	// namespace Menge