#	include <boost/intrusive_ptr.hpp>

#	include <string>
#	include <set>

#	include <vector>
#	include <algorithm>
#	include <functional>

#	include <stdio.h>
#	include <typeinfo>
#	include <utility>

struct ConstStringHolder
{
public:
	ConstStringHolder( const std::string & _value )
		: m_value(_value)
		, m_reference(0)
	{
	}

public:
	const std::string & str() const
	{
		return m_value;
	}

public:
	std::size_t reference() const
	{
		return m_reference;
	}

protected:
	std::size_t m_reference;
	std::string m_value;

	friend inline void intrusive_ptr_add_ref( ConstStringHolder * _ptr )
	{
		++_ptr->m_reference;
	}

	friend inline void intrusive_ptr_release( ConstStringHolder * _ptr )
	{
		if( --_ptr->m_reference == 0 )
		{
			delete _ptr;
		}
	}

};

typedef boost::intrusive_ptr<ConstStringHolder> ConstStringHolderPtr;

struct ConstString
{
public:
	ConstString( const char * _str )
		: m_holder( new ConstStringHolder(_str) )
		, m_next(0)
		, m_prev(0)
	{
	}

	ConstString( const std::string & _str )
		: m_holder( new ConstStringHolder(_str) )
		, m_next(0)
		, m_prev(0)
	{
	}

	ConstString( const ConstString & _cstr )
		: m_holder(_cstr.m_holder)
	{
		if( _cstr.m_next )
		{
			_cstr.m_next->m_prev = this;
			m_next = _cstr.m_next;
		}
		else
		{
			m_next = 0;
		}

		m_prev = &_cstr;
		_cstr.m_next = this;
	}

	~ConstString()
	{
		if( m_next )
		{
			m_next->m_prev = m_prev;
		}

		if( m_prev )
		{
			m_prev->m_next = m_next;
		}
	}

public:
	const char * c_str() const
	{
		return m_holder->str().c_str();
	}

public:
	bool operator == ( const ConstString & _cstr ) const
	{
		if( m_holder == _cstr.m_holder )
		{
			return true;
		}

		if( m_holder->str() != _cstr.m_holder->str() )
		{
			return false;
		}

		this->combine( _cstr );

		return true;
	}

	bool operator != ( const ConstString & _cstr ) const
	{
		return !this->operator == (_cstr);
	}

	friend bool operator < ( const ConstString & _left, const ConstString & _right )
	{
		if( _left.m_holder.get() == _right.m_holder.get() )
		{
			return false;
		}

		int res = _left.m_holder->str().compare( _right.m_holder->str() );

		if( res == 0 )
		{
			_left.combine( _right );

			return false;
		}

		return res < 0;
	}

	void combine( const ConstString & _cstr ) const
	{
		if( m_holder->reference() < _cstr.m_holder->reference() )
		{
			m_holder = _cstr.m_holder;

			upfill( m_next, _cstr.m_holder );
			downfill( m_prev, _cstr.m_holder );

			link( &_cstr, this );
		}
		else
		{
			_cstr.m_holder = m_holder;

			upfill( _cstr.m_next, m_holder );
			downfill( _cstr.m_prev, m_holder );

			link( this, &_cstr );
		}
	}

	friend const ConstString * downcast( const ConstString * _it )
	{
		while( _it->m_prev )
		{
			_it = _it->m_prev;
		}

		return _it;
	}

	friend const ConstString * upcast( const ConstString * _it )
	{
		while( _it->m_next )
		{
			_it = _it->m_next;
		}

		return _it;
	}

	friend void upfill( const ConstString * _it, const ConstStringHolderPtr & _holder )
	{
		while( _it )
		{
			_it->m_holder = _holder;
			_it = _it->m_next;
		}
	}

	friend void downfill( const ConstString * _it, const ConstStringHolderPtr & _holder )
	{
		while( _it )
		{
			_it->m_holder = _holder;
			_it = _it->m_prev;
		}
	}

	friend void link( const ConstString * _left, const ConstString * _right )
	{
		const ConstString * middle = _right;
		const ConstString * front = downcast( middle );
		const ConstString * back = upcast( middle );

		front->m_prev = _left;

		if( _left->m_next )
		{
			_left->m_next->m_prev = back;
		}

		back->m_next = _left->m_next;
		_left->m_next = front;
	}

protected:
	mutable ConstStringHolderPtr m_holder;
	mutable const ConstString * m_next;
	mutable const ConstString * m_prev;
};


void main()
{
	std::string a;
	std::string b;
	 a < b;

	ConstString host = "domainname.com";
	ConstString host2 = "domainname.com";

	std::set<ConstString> s;

	s.insert( host );
	s.insert( host2 );

	if( host == host2 )
	{
		printf("1");
	}

	const char* ss = strchr( host.c_str(), '.' );
	std::ptrdiff_t hostLenght = std::distance( host.c_str(), ss );

	printf("%d",hostLenght);
}