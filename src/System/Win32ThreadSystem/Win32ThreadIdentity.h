#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "WIN32/WindowsIncluder.h"

#include "Core/ServantBase.h"

namespace Mengine
{
	class Win32ThreadIdentity
		: public ServantBase<ThreadIdentityInterface>
	{
	public:
		Win32ThreadIdentity();

	public:
        bool initialize( const ThreadMutexInterfacePtr & _mutex, int _priority, const Char * _file, uint32_t _line );

	public:
		void main();

	public:
		bool processTask( ThreadTaskInterface * _task ) override;
		bool completeTask() override;

	public:
		void join() override;

    protected:
		ThreadMutexInterfacePtr m_mutex;

		HANDLE m_handle;

#ifdef _DEBUG
		const Char * m_file;
		uint32_t m_line;
#endif
			
        ThreadTaskInterface * m_task;

		volatile bool m_complete;
		volatile bool m_exit;
	};
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<Win32ThreadIdentity> Win32ThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
