#	include "ThreadTaskPrefetch.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskPrefetch::ThreadTaskPrefetch()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetch::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetch::initialize( const ConstString& _pakName, const FilePath & _fileName )
	{
		m_pakName = _pakName;
		m_filePath = _fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPrefetch::_onComplete( bool _successful )
	{
		(void) _successful;

		m_group = nullptr;
		m_stream = nullptr;
	}
}
