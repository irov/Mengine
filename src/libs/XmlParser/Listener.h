#	pragma once

namespace XMLParser
{
	class Element;

	class Listener
	{
	public:
		virtual void loader( Element * _element  ) = 0;
	};
}