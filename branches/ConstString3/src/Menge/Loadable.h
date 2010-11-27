#	pragma once

namespace Menge
{
	class BinParser;

	class Loadable
	{
	public:
		virtual void loader( BinParser * _parser ) = 0;
		
	public:
		void loaded();

	protected:
		virtual void _loaded() = 0;
	};
}