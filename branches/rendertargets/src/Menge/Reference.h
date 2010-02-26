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
		virtual unsigned int incrementReference();

		//! ��������� ������� �� �������
		/*!
			return ���������� ������.
		*/
		virtual unsigned int decrementReference();

		//! ���������� ������
		/*!
			return ���������� ������.
		*/
		virtual unsigned int countReference();

	public:
		virtual bool _incrementZero();
		virtual void _decrementZero();

	protected:
		unsigned int m_refcounter;
	};
}
