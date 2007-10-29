#	pragma once

namespace Menge
{
	class Reference
	{
	public:
		Reference();

	public:
		virtual void incrementReference();
		virtual size_t decrementReference();
		virtual size_t countReference();

	public:
		virtual void _incrementZero();
		virtual void _decrementZero();

	protected:
		size_t m_refcounter;
	};
}