#	include "ThreadTaskPrefetchDataflow.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskPrefetchDataflow::ThreadTaskPrefetchDataflow()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetchDataflow::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetchDataflow::initialize( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _dataflowType )
	{
		m_pakName = _pakName;
		m_fileName = _fileName;
		m_dataflowType = _dataflowType;
	}
	//////////////////////////////////////////////////////////////////////////
	const DataInterfacePtr & ThreadTaskPrefetchDataflow::getData() const
	{
		return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskPrefetchDataflow::_onRun()
	{
		m_stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( m_pakName, m_fileName );

		if( m_stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetchDataflow::_onRun: invalide open file '%s':'%s'"
				, m_pakName.c_str()
				, m_fileName.c_str()
				);

			return false;
		}
		
		m_dataflow = DATA_SERVICE(m_serviceProvider)
			->getDataflow( m_dataflowType );
			
		if( m_dataflow == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetchDataflow::_onRun: '%s':'%s' invalide get dataflow '%s'"
				, m_pakName.c_str()
				, m_fileName.c_str()
				, m_dataflowType.c_str()
				);

			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskPrefetchDataflow::_onMain()
	{
		m_data = m_dataflow->load( m_stream );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetchDataflow::_onComplete( bool _successful )
	{
		if( _successful == false )
		{
			return;
		}

		m_stream = nullptr;
		m_dataflow = nullptr;
	}
}
