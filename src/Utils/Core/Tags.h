#	pragma once

#	include "Core/ConstString.h"

#	include "stdex/stl_vector.h"
#	include "stdex/stl_string.h"

namespace Menge
{
	typedef stdex::vector<ConstString> TVectorConstString;

	class Tags
	{
	public:
		void addTag( const ConstString & _tag );		
		bool hasTag( const ConstString & _tag ) const;
		bool inTags( const Tags & _tag ) const;

		void clear();

	public:
		const TVectorConstString & getTags() const;
		
	public:
		void swap( Tags & _tags );

	public:
		stdex::string to_str() const;

	protected:		
		TVectorConstString m_tags;
	};
}

namespace std
{
	template<> inline void swap( Menge::Tags & _left, Menge::Tags & _right )
	{
		_left.swap( _right );
	}
}