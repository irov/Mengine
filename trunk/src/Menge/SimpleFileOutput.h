/*
 *	FileOutput.h
 *
 *	Created by _Berserk_ on 2.9.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "FileOutput.h"

namespace Menge
{
	class FileSystem;

	class SimpleFileOutput
		: public FileOutput
	{
	public:
		SimpleFileOutput();
		~SimpleFileOutput();

		void loadStream( OutputStreamInterface* _iStream );
		OutputStreamInterface* unloadStream();

		void write( const void* _data, int _count ) override;
		void flush() override;
		int tell() override;

	private:
		OutputStreamInterface* m_iStream;
		int m_iStreamCursor;
	};
}	// namespace Menge
