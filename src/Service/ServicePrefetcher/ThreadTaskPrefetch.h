#	pragma once

#	include "Interface/PrefetcherInterface.h"
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
        ~ThreadTaskPrefetch();
	
	public:
		void initialize( const ConstString& _pakName, const FilePath & _fileName, const PrefetcherObserverInterfacePtr & _observer );

	protected:
        void _onPreparation() override;
        bool _onRun() override;
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
        PrefetcherObserverInterfacePtr m_observer;
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