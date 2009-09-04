/*
 *	BufferedFileInput.h
 *
 *	Created by _Berserk_ on 26.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "FileInterface.h"

namespace Menge
{
	class FileSystem;

	class BufferedFileInput
		: public FileInputInterface
	{
	public:
		BufferedFileInput();
		~BufferedFileInput();

		void loadStream( FileSystem* _fileSystem, InputStreamInterface* _iStream );
		InputStreamInterface* unloadStream();

		void close() override;
		int tell() override;
		int read( void* _buf, int _count ) override;
		void seek( int _pos ) override;
		int size() const override;

	private:
		FileSystem* m_fileSystem;
		InputStreamInterface* m_iStream;
		std::vector<unsigned char> m_buffer;
		int m_bufferBegin;
		int m_iStreamCursor;
		int m_iStreamSize;
		int m_bufferMaxSize;
	};
}	// namespace Menge
