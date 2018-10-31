#pragma once

#include "ResourceCursor.h"

namespace Mengine
{
    class ResourceCursorSystem
        : public ResourceCursor
    {
        DECLARE_VISITABLE( ResourceCursor );

    public:
        ResourceCursorSystem();
		~ResourceCursorSystem() override;

	public:
		void setFilePath( const FilePath & _filePath );

    public:
        const FilePath & getFilePath() const override;
        const MemoryInterfacePtr & getBuffer() const override;

    protected:
        FilePath m_filePath;

        MemoryInterfacePtr m_buffer;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ResourceCursorSystem> ResourceCursorSystemPtr;
	//////////////////////////////////////////////////////////////////////////
}