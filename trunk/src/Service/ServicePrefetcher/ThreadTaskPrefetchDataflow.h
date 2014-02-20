#	pragma once

#	include "Interface/DataInterface.h"

#	include "Kernel/ThreadTask.h"

#	include "Core/MemoryInput.h"

#	include "Factory/FactoryDefault.h"

namespace Menge
{
	class ThreadTaskPrefetchDataflow
		: public ThreadTask
	{
	public:
		ThreadTaskPrefetchDataflow();
		
	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		void initialize( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _dataflowType );

	public:
		const DataInterfacePtr & getData() const;

	protected:
		bool _onRun() override;

	protected:
		bool _onMain() override;

	protected:
		void _onComplete( bool _successful ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_pakName;
		FilePath m_fileName;
		ConstString m_dataflowType;
		
	protected:
		InputStreamInterfacePtr m_stream;
		DataflowInterfacePtr m_dataflow;
		
		DataInterfacePtr m_data;
	};

	typedef stdex::intrusive_ptr<ThreadTaskPrefetchDataflow> ThreadTaskPrefetchDataflowPtr;
}