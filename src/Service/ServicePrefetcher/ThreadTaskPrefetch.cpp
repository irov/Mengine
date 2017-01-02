#	include "ThreadTaskPrefetch.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskPrefetch::ThreadTaskPrefetch()
	{
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
