#	pragma once

namespace Menge
{
	class XmlElement;
	class BinParser;

	class Loadable
	{
	public:
		virtual void loader( XmlElement * _element ) = 0;
		virtual void parser( BinParser * _parser ) = 0;

	protected:
		virtual void _loaded() = 0;
	};
}