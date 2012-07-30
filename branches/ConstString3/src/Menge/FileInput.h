/*
 *	FileInput.h
 *
 *	Created by _Berserk_ on 7.9.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class FileGroup;

	class FileInput
		: public FileInputStreamInterface
	{
	public:
		FileInput();
		virtual ~FileInput();

	public:
		void setFileSystem( FileGroup * _fileSystem );
		FileGroup * getFileSystem() const;

	public:
		bool open( const WString & _filename ) override;
		void close() override;

	protected:
		FileGroup * m_fileSystem;
		bool m_opened;
	};
}	// namespace Menge
