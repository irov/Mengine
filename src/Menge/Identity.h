#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class Identity
	{
	public:
		//! Node pure virtual. set node name
		/*!
		\param _name the first argument.
		*/
		void setName(const String & _name);

		//! Node pure virtual. get node name
		/*!
		\return name
		*/
		const String & getName() const;

		//! Node pure virtual. set node type
		/*!
		\param type the first argument.
		*/
		void setType( const String & _type );

		//! Node pure virtual. get node type
		/*!
		\return type
		*/
		const String & getType() const;

	protected:
		String m_name;
		String m_type;
	};
}