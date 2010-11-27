#	pragma once

namespace Menge
{
	//! Reference - класс для подсчета ссылок.

	class Reference
	{
	public:
		Reference();

	public:
		virtual unsigned int incrementReference();
		virtual unsigned int decrementReference();

		virtual unsigned int countReference();

	public:
		virtual bool _incrementZero();
		virtual void _decrementZero();

	protected:
		unsigned int m_refcounter;
	};
}
