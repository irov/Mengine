#	pragma once

#	include "Config/Typedef.h"

#	include <map>

namespace Menge
{
	class Factory
	{
	public:
		typedef void * (*TGenFunc)( void * );

	public:
		void unregistration( const String & _key );
		void registration( const String & _key, TGenFunc _func );
		void * generate( const String & _name, void * _data  );

		template<class T, class A>
		T * generate_t( const String & _name, A & _data )
		{
			return static_cast<T*>( this->generate( _name, (void *)&_data ) );
		}

	protected:
		typedef std::map<String, TGenFunc> TMapGenerator;
		TMapGenerator m_generator;
	};
}
