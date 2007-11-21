#	pragma once

namespace Menge
{
	//! Reference - ����� ��� �������� ������.

	class Reference
	{
	public:
		Reference();

	public:
		//! ����������� ������� �� �������
		/*!
		*/
		virtual void incrementReference();

		//! ��������� ������� �� �������
		/*!
			return ���������� ������.
		*/
		virtual size_t decrementReference();

		//! ���������� ������
		/*!
			return ���������� ������.
		*/
		virtual size_t countReference();

	public:
		virtual void _incrementZero();
		virtual void _decrementZero();

	protected:
		size_t m_refcounter;
	};
}