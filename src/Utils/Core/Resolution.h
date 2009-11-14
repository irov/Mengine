#	pragma once

#	include <cstddef>

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

		float getRatio() const;

	public:
		std::size_t & operator [] ( int i );
		std::size_t operator [] ( int i ) const;
		bool operator == ( const Resolution & _resolution ) const;

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
