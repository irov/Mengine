/*
 *	ThreadSystemInterface.h
 *
 *	Created by _Berserk_ on 24.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

namespace Menge
{
	class ThreadInterface
	{
	public:
		virtual void main() = 0;
		virtual bool isComplete() const = 0;
	};

	class ThreadSystemInterface
	{
	public:
		virtual void createThread( ThreadInterface * _thread ) = 0;
		virtual void joinThread( ThreadInterface* _thread ) = 0;

	public:
		virtual void startMutex() = 0;
		virtual void stopMutex() = 0;
	};
}

bool	initInterfaceSystem( Menge::ThreadSystemInterface** );
void	releaseInterfaceSystem( Menge::ThreadSystemInterface* );
