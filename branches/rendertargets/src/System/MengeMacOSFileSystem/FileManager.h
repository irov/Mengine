#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class DataStream;

	class FileManager
	{
	public:
		FileManager( const Menge::Text& _initPath );
		~FileManager();

	public:
		void setInitPath( const Menge::Text& _path );
		bool isCaseSensitive() const;

		DataStream* open( const Menge::Text& _filename ) const;

	protected:
		Menge::Text m_initPath;
	};
}