#	include "Resource.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Resource::Resource()
		: m_compile(false)
	{}
	//////////////////////////////////////////////////////////////////////////
	bool Resource::compile()
	{
		if( m_compile )
		{
			return true;
		}

		m_compile = true;

		if( _compile() == false )
		{
			//Force release
			release();
		}

		return m_compile;
	}
	//////////////////////////////////////////////////////////////////////////
	void Resource::release()
	{
		if( m_compile )
		{
			m_compile = false;
			_release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Resource::_compile()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Resource::_release()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Resource::recompile()
	{
		if( isCompile() == false )
		{
			return true;
		}

		_release();

		if( _compile() == false )
		{
			release();

			return false;
		}

		return true;
	}
}
