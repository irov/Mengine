#pragma once

#include "Kernel/ConstString.h"

#include "Config/Vector.h"
#include "Config/String.h"

namespace Mengine
{
	typedef Vector<ConstString> TVectorConstString;

	class Tags
	{
    public:
        Tags();
        ~Tags();

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
		String to_str() const;

	protected:		
		TVectorConstString m_tags;
	};
}

namespace std
{
	template<> inline void swap( Mengine::Tags & _left, Mengine::Tags & _right )
	{
		_left.swap( _right );
	}
}