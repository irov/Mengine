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
	class FileGroupInterface;

	class FileInput
		: public FileInputStreamInterface
	{
	public:
		FileInput();
		virtual ~FileInput();

	public:
		void setFileSystem( FileGroupInterface * _fileSystem );
		FileGroupInterface * getFileSystem() const;

	public:
		bool open( const WString & _filename ) override;
		void close() override;

	protected:
		FileGroupInterface * m_fileSystem;
		bool m_opened;
	};
}	// namespace Menge
