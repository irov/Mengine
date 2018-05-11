#pragma once

#include "Interface/DataInterface.h"
#include "Interface/FileSystemInterface.h"

#include "ThreadTaskPrefetch.h"

namespace Mengine
{
	class ThreadTaskPrefetchDataflow
		: public ThreadTaskPrefetch
	{
	public:
		ThreadTaskPrefetchDataflow();
		
	public:
		void setDataflowType( const ConstString & _dataflowType );

	public:
		const DataInterfacePtr & getData() const;

	protected:
		bool _onRun() override;
		bool _onMain() override;
		void _onComplete( bool _successful ) override;
		
	protected:
		ConstString m_dataflowType;
		DataflowInterfacePtr m_dataflow;
		
	protected:		
		DataInterfacePtr m_data;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ThreadTaskPrefetchDataflow> ThreadTaskPrefetchDataflowPtr;
}