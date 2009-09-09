/*
 *	FilePackZip.h
 *
 *	Created by _Berserk_ on 16.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "FilePack.h"
#	include <map>

namespace Menge
{
	class MutexInterface;

	class FilePackZip
		: public FilePack
	{
	public:
		FilePackZip( const String& _filename, DataStreamInterface* _stream );
		~FilePackZip();

		DataStreamInterface* openFile( const String& _filename ) override;
		bool hasFile( const String& _filename ) override;

	protected:
		String m_filename;

		void parsePack_();

		struct FileInfo
		{
			size_t seek_pos;
			size_t file_size;
			size_t unz_size;
			uint16 compr_method;
		};

		typedef std::map< String, FileInfo > TFileInfoMap;
		TFileInfoMap m_files;
		MutexInterface* m_streamMutex;
	};
}	// namespace Menge
