#	include "Resource.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Resource::Resource()
		: m_compile(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Resource::compile()
	{
		if( m_compile == true )
		{
			return true;
		}

		m_compile = true;

		if( this->_compile() == false )
		{
			//Force release
			//release();
			m_compile = false;

			return false;
		}

		return m_compile;
	}
	//////////////////////////////////////////////////////////////////////////
	void Resource::release()
	{
		if( m_compile == false )
		{
			return;
		}
		
		m_compile = false;

		this->_release();
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int Resource::memoryUse() const
	{
		return 0;
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
		if( this->isCompile() == false )
		{
			return true;
		}

		this->_release();

		if( this->_compile() == false )
		{		
			m_compile = false;

			return false;
		}

		this->_recompile();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Resource::_recompile()
	{
		//Empty
	}
}
