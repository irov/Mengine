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
	class FilePackZip
		: public FilePack
	{
	public:
		FilePackZip( DataStreamInterface* _stream );
		~FilePackZip();

		DataStreamInterface* openFile( const String& _filename ) override;
		bool hasFile( const String& _filename ) override;

	protected:
		void parsePack_();

		struct FileInfo
		{
			size_t seek_pos;
			size_t file_size;
		};

		typedef std::map< String, FileInfo > TFileInfoMap;
		TFileInfoMap m_files;
	};
}	// namespace Menge
