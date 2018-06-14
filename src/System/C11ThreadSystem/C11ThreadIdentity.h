#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "Core/ServantBase.h"

#include <atomic>
#include <thread>
#include <mutex>

namespace Mengine
{
	class C11ThreadIdentity
		: public ServantBase<ThreadIdentityInterface>
	{
	public:
		C11ThreadIdentity();
        ~C11ThreadIdentity() override;

	public:
        bool initialize( int _priority, const Char * _file, uint32_t _line );

	public:
		void main();

	public:
		bool processTask( ThreadTaskInterface * _task ) override;
		bool completeTask() override;

	public:
		void join() override;

    protected:
		std::mutex m_mutex;

        std::thread m_thread;

        ThreadTaskInterface * m_task;

        std::atomic_bool m_complete;
        std::atomic_bool m_exit;

#ifndef NDEBUG
        const Char * m_file;
        uint32_t m_line;
#endif
	};
    //////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<C11ThreadIdentity> C11ThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
