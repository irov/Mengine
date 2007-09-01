#	pragma once

namespace Menge
{
	class Event
	{
	public:
		virtual ~Event(){};
	public:
		virtual void call( const char * format, ... ) = 0;
	};
}