# pragma once

#include "Interface/MemoryInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Core/ConstString.h"
#include "Core/String.h"
#include "Core/FilePath.h"

#include "Factory/Factorable.h"

namespace Mengine
{
	class TextLocalePack
		: public Factorable
	{
	public:
		TextLocalePack();
		~TextLocalePack() override;

	public:
		bool initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _path );
		
	public:
		const FileGroupInterfacePtr & getFileGroup() const;
		const FilePath & getPath() const;

	public:
		MemoryInterfacePtr getXmlBuffer() const;

	protected:
        FileGroupInterfacePtr m_fileGroup;
		FilePath m_path;

		MemoryBufferInterfacePtr m_memory;		
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<TextLocalePack> TextLocalePackPtr;
}
