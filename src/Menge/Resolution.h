#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class Resolution
	{
	public:
		Resolution();
		Resolution( std::size_t _width, std::size_t _height );

	public:
		void setWidth( std::size_t _width );
		std::size_t getWidth() const;

		void setHeight( std::size_t _height );
		std::size_t getHeight() const;

	public:
		std::size_t & operator [] ( int i )
		{
			return m_size[ i ];
		}

		std::size_t operator [] ( int i ) const
		{
			return m_size[ i ];
		}

	protected:
		union
		{
			struct  
			{
				std::size_t m_width;
				std::size_t m_height;
			};

			std::size_t m_size[2];
		};
	};
}