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
		BufferedFileInput( FileSystem* _fileSystem );
		~BufferedFileInput();

		void loadStream( DataStreamInterface* _iStream );
		DataStreamInterface* unloadStream();

		void close() override;
		int read( void* _buf, int _count ) override;
		void seek( int _pos ) override;
		int size() const override;

	private:
		FileSystem* m_fileSystem;
		DataStreamInterface* m_iStream;
		std::vector<unsigned char> m_buffer;
		int m_bufferBegin;
		int m_iStreamCursor;
		int m_iStreamSize;
	};
}	// namespace Menge
