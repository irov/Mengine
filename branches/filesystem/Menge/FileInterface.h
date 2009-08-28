/*
 *	FileInterface.h
 *
 *	Created by _Berserk_ on 27.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class FileInputInterface
		: public InputStreamInterface
	{
	public:
		virtual void close() = 0;
		virtual int tell() = 0;
	};

	class FileOutputInterface
		: public OutputStreamInterface
	{
	public:
		virtual void close() = 0;
		virtual int tell() = 0;
	};
}	// namespace Menge
