#	pragma once

namespace Menge
{
	class BinParser;

	class Loadable
	{
	public:
		virtual void loader( BinParser * _parser ) = 0;

	public:
		virtual void _loaded() = 0;
	};
}