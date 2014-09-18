#	pragma once

#	include "Interface/DataInterface.h"
#	include "Interface/FileSystemInterface.h"

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
		FilePath m_filePath;
		ConstString m_dataflowType;
		
	protected:
		FileGroupInterfacePtr m_group;
		InputStreamInterfacePtr m_stream;
		DataflowInterfacePtr m_dataflow;
		
	protected:
		DataInterfacePtr m_data;
	};

	typedef stdex::intrusive_ptr<ThreadTaskPrefetchDataflow> ThreadTaskPrefetchDataflowPtr;
}