/*
 *	FileInput.h
 *
 *	Created by _Berserk_ on 7.9.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "FileInterface.h"

namespace Menge
{
	class FileSystem;

	class FileInput
		: public FileInputInterface
	{
	public:
		FileInput();
		virtual ~FileInput();

		void setFileSystem( FileSystem* _fileSystem );
		FileSystem* getFileSystem();

		bool open( const String& _filename );
		void close();

	protected:
		FileSystem* m_fileSystem;
		bool m_opened;
	};
}	// namespace Menge
