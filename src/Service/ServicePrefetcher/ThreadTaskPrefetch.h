#	pragma once

#	include "Interface/StreamInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "Kernel/ThreadTask.h"

namespace Menge
{
	class ThreadTaskPrefetch
		: public ThreadTask
	{
	public:
		ThreadTaskPrefetch();
	
	public:
		void initialize( const ConstString& _pakName, const FilePath & _fileName );

	protected:
		void _onComplete( bool _successful ) override;
		
	public:
		inline const ConstString & getPakName() const;
		inline const FilePath & getFilePath() const;
		
	protected:
		ConstString m_pakName;
		FilePath m_filePath; 
				
	protected:
		FileGroupInterfacePtr m_group;
		InputStreamInterfacePtr m_stream;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & ThreadTaskPrefetch::getPakName() const
	{
		return m_pakName;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const FilePath & ThreadTaskPrefetch::getFilePath() const
	{
		return m_filePath;
	}
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskPrefetch> ThreadTaskPrefetchPtr;
}