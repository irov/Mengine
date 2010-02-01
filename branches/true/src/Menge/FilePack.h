/*
 *	FilePack.h
 *
 *	Created by _Berserk_ on 16.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class DataStreamInterface;

	class FilePack
	{
	public:
		FilePack( DataStreamInterface* _stream );
		virtual ~FilePack();

		size_t getFileCount() const;

		virtual DataStreamInterface* openFile( const String& _filename ) = 0;
		virtual bool hasFile( const String& _filename ) = 0;
		
	protected:
		DataStreamInterface* m_stream;
		size_t m_fileCount;
	};

}	// namespace Menge
