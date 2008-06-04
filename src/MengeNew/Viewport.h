#	pragma once

#	include "Math/vec2.h"
#	include <string>

namespace Menge
{
	class Viewport
	{
	public:
		Viewport();
		Viewport( const mt::vec2f & _begin, const mt::vec2f & _end );

	public:
		bool testPoint( const mt::vec2f & _point ) const;
		bool testRectangle( const mt::vec2f & _min,  const mt::vec2f & _max ) const;

	public:
		void parallax( const mt::vec2f & _factor );

		//void setCamera( const std::string& _name );
		//const std::string& getCamera() const;

	public:
		mt::vec2f begin;
		mt::vec2f end;
		//std::string m_camera;
	};
}
