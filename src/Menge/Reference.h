#	pragma once

namespace Menge
{
	//! Reference - класс для подсчета ссылок.

	class Reference
	{
	public:
		Reference();

	public:
		//! Увеличивает счетчик на еденицу
		/*!
		*/
		virtual void incrementReference();

		//! Уменьшает счетчик на еденицу
		/*!
			return количество ссылок.
		*/
		virtual size_t decrementReference();

		//! Количество ссылок
		/*!
			return количество ссылок.
		*/
		virtual size_t countReference();

	public:
		virtual void _incrementZero();
		virtual void _decrementZero();

	protected:
		size_t m_refcounter;
	};
}