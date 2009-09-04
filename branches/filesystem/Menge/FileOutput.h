/*
 *	FileOutput.h
 *
 *	Created by _Berserk_ on 2.9.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "FileInterface.h"

namespace Menge
{
	class FileSystem;

	class FileOutput
		: public FileOutputInterface
	{
	public:
		FileOutput();
		~FileOutput();

		void loadStream( FileSystem* _fileSystem, OutputStreamInterface* _iStream );
		OutputStreamInterface* unloadStream();

		void write( const void* _data, int _count ) override;
		void flush() override;
		void close() override;
		int tell() override;

	private:
		FileSystem* m_fileSystem;
		OutputStreamInterface* m_iStream;
		int m_iStreamCursor;
	};
}	// namespace Menge
