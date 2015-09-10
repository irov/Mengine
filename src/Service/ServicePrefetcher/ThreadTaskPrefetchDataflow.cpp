#	include "ThreadTaskPrefetchDataflow.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskPrefetchDataflow::ThreadTaskPrefetchDataflow()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetchDataflow::setDataflowType( const ConstString & _dataflowType )
	{
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
		m_group = FILE_SERVICE( m_serviceProvider )
			->getFileGroup( m_pakName );

		if( m_group == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetchDataflow::_onRun can't get group '%s'"
				, m_pakName.c_str()
				);

			return false;
		}

		m_stream = m_group->createInputFile( m_filePath, false );

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
				, m_filePath.c_str()
				, m_dataflowType.c_str()
				);

			return false;
		}

		m_data = m_dataflow->create();

		if( m_data == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetchDataflow::_onRun: '%s':'%s' dataflow invalid create data '%s'"
				, m_pakName.c_str()
				, m_filePath.c_str()
				, m_dataflowType.c_str()
				);

			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskPrefetchDataflow::_onMain()
	{
		if( m_group->openInputFile( m_filePath, m_stream, 0, 0, false ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPrefetcherTextureDecoder::_onRun: invalide open file '%s':'%s'"
				, m_pakName.c_str()
				, m_filePath.c_str()
				);

			return false;
		}

		bool successful = m_dataflow->load( m_data, m_stream );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetchDataflow::_onComplete( bool _successful )
	{
		ThreadTaskPrefetch::_onComplete( _successful );

		m_dataflow = nullptr;
	}
}
