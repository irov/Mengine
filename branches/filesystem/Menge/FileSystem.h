/*
 *	FileSystem.h
 *
 *	Created by _Berserk_ on 27.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Factorable.h"

namespace Menge
{
	class FileSystem
		: public Factorable
	{
	public:
		virtual bool initialize( const String& _path, bool _create ) = 0;
		virtual bool existFile( const String& _filename ) = 0;
		virtual FileInputInterface* openInputFile( const String& _filename ) = 0;
		virtual void closeInputFile( FileInputInterface* _file ) = 0;
		virtual FileOutputInterface* openOutputFile( const String& _filename ) { return 0; }
		virtual void closeOutputFile( FileOutputInterface* _outStream ) {}
	};
}	// namespace Menge
