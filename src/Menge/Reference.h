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
		virtual unsigned int incrementReference();

		//! Уменьшает счетчик на еденицу
		/*!
			return количество ссылок.
		*/
		virtual unsigned int decrementReference();

		//! Количество ссылок
		/*!
			return количество ссылок.
		*/
		virtual unsigned int countReference();

	public:
		virtual bool _incrementZero();
		virtual void _decrementZero();

	protected:
		unsigned int m_refcounter;
	};
}