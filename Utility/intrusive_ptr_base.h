#	pragma once


template<class T>
struct intrusive_ptr_base
{
	virtual ~intrusive_ptr_base()
	{

	}

	void addRefCnt()
	{
		++m_refCnt;
	}

	void releaseRefCnt()
	{
		if(m_refCnt == 1)
		{
			delete this;
		}
		else
		{
			--m_refCnt;
		}
	}

	size_t refCnt()
	{
		return m_refCnt;
	}
};