/*
 *	BufferedFileInput.h
 *
 *	Created by _Berserk_ on 26.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "FileInput.h"

namespace Menge
{
	class FileSystem;

	class BufferedFileInput
		: public FileInput
	{
	public:
		BufferedFileInput();
		~BufferedFileInput();

	public:
		void loadStream( InputStreamInterface* _iStream );
		InputStreamInterface* unloadStream();

	public:
		int tell() override;
		int read( void* _buf, int _count ) override;
		void seek( int _pos ) override;
		int size() const override;

	protected:
		InputStreamInterface* m_iStream;

		TBlobject m_buffer;
		int m_bufferBegin;
		int m_iStreamCursor;
		int m_iStreamSize;
		int m_bufferMaxSize;
	};
}	// namespace Menge
