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
		m_group = FILE_SERVICE(m_serviceProvider)
			->getFileGroup( m_pakName );

		if( m_group == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetchDataflow::_onRun can't get group '%s'"
				, m_pakName.c_str()
				);

			return false;
		}

		m_stream = m_group->createInputFile();

		if( m_stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetchDataflow::_onRun can't create input file '%s'"
				, m_pakName.c_str()
				);

			return nullptr;
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

		m_data = m_dataflow->create();

		if( m_data == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetchDataflow::_onRun: '%s':'%s' dataflow invalid create data '%s'"
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
		if( m_group->openInputFile( m_fileName, m_stream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetcherTextureDecoder::_onRun: invalide open file '%s':'%s'"
				, m_pakName.c_str()
				, m_fileName.c_str()
				);

			return false;
		}

		bool successful = m_dataflow->load( m_data, m_stream );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetchDataflow::_onComplete( bool _successful )
	{
		if( _successful == false )
		{
			return;
		}

		m_group = nullptr;
		m_stream = nullptr;
		m_dataflow = nullptr;
	}
}
