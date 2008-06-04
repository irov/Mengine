#	pragma once

#	include <string>

namespace Menge
{
	class Identity
	{
	public:
		//! Node pure virtual. set node name
		/*!
		\param _name the first argument.
		*/
		void setName(const std::string & _name);

		//! Node pure virtual. get node name
		/*!
		\return name
		*/
		const std::string & getName() const;

		//! Node pure virtual. set node type
		/*!
		\param type the first argument.
		*/
		void setType( const std::string & _type );

		//! Node pure virtual. get node type
		/*!
		\return type
		*/
		const std::string & getType() const;

	protected:
		std::string m_name;
		std::string m_type;
	};
}