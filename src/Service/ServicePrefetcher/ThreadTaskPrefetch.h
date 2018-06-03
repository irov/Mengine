#pragma once

#include "Interface/PrefetcherInterface.h"
#include "Interface/StreamInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Kernel/ThreadTask.h"

namespace Mengine
{
	class ThreadTaskPrefetch
		: public ThreadTask
	{
	public:
		ThreadTaskPrefetch();
        ~ThreadTaskPrefetch() override;
	
	public:
		void initialize( const FileGroupInterfacePtr& _pakName, const FilePath & _fileName, const PrefetcherObserverInterfacePtr & _observer );

	protected:
        void _onPreparation() override;
        bool _onRun() override;
        void _onCancel() override;
		void _onComplete( bool _successful ) override;
		
	public:
		inline const FileGroupInterfacePtr & getFileGroup() const;
		inline const FilePath & getFilePath() const;
		
	protected:
		FilePath m_filePath;
				
	protected:
		FileGroupInterfacePtr m_fileGroup;
		InputStreamInterfacePtr m_stream;
        PrefetcherObserverInterfacePtr m_observer;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const FileGroupInterfacePtr & ThreadTaskPrefetch::getFileGroup() const
	{
		return m_fileGroup;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const FilePath & ThreadTaskPrefetch::getFilePath() const
	{
		return m_filePath;
	}
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ThreadTaskPrefetch> ThreadTaskPrefetchPtr;
}