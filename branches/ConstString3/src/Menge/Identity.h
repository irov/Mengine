#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class FactoryIdentity;

	class Identity
	{
	public:
		Identity();

	public:
		void setFactoryIdentity( FactoryIdentity * _factoryIdentity );

	public:
		void setName( const String & _name );
		const String & getName() const;

		void setType( const String & _type );
		const String & getType() const;

		inline std::size_t getNameIdentity() const;
		inline std::size_t getTypeIdentity() const;

	protected:
		FactoryIdentity * m_factoryIdentity;

		std::size_t m_nameIdentity;
		std::size_t m_typeIdentity;
	};
	//////////////////////////////////////////////////////////////////////////
	inline std::size_t Identity::getNameIdentity()const
	{	
		return m_nameIdentity;
	}
	//////////////////////////////////////////////////////////////////////////
	inline std::size_t Identity::getTypeIdentity() const
	{
		return m_typeIdentity;
	}
}
