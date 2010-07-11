#	pragma once

#	include "Core/Holder.h"
#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

#	include "Utils/Archive/ArchiveRead.hpp"

namespace Menge
{
	class Loadable;

	class LoaderEngine
		: public Holder<LoaderEngine>
	{
	public:
		bool load( const ConstString & _pak, const String & _path, Loadable * _loadable );
		bool loadBinary( const Archive & _blob, Loadable * _loadable );

	public:
		bool import( const ConstString & _pak, const String & _path, Archive & _archive ); 

	protected:
		Archive m_bufferArchive;
	};
}