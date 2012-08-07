#	pragma once

namespace Metabuf
{
    class Metadata;
}

namespace Menge
{
	class BinParser;

	class Loadable
	{
	public:
		virtual void loader( const Metabuf::Metadata * _parser ) = 0;
		
	public:
		void loaded();

	protected:
		virtual void _loaded();
	};
}