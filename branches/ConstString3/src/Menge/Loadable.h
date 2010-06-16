#	pragma once

class XmlElement;

namespace Menge
{
	class BinParser;

	class Loadable
	{
	public:
		virtual void loader( XmlElement * _element ) = 0;
		virtual void parser( BinParser * _parser ) = 0;
	};
}