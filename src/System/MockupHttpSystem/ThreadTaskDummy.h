#	pragma once

#	include "Interface/HttpSystemInterface.h"
#	include "Interface/StreamInterface.h"

#	include "Kernel/ThreadTask.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ThreadTaskDummy
		: public ThreadTask
	{
	public:
		ThreadTaskDummy();
		
	public:
		bool initialize( uint32_t _type, HttpRequestID _id, HttpReceiver * _receiver );

	protected:
		bool _onRun() override;

	protected:
		bool _onMain() override;

	protected:
		void _onComplete( bool _successful ) override;

	protected:		
		uint32_t m_type;
		HttpRequestID m_id;
		HttpReceiver * m_receiver;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskDummy> ThreadTaskDummyPtr;
	//////////////////////////////////////////////////////////////////////////
}