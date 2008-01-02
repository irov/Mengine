#	pragma once

#	include <string>

namespace Menge
{
	//! Reference - ��������� ��� �������.

	class Resource
	{
	public:
		//! �����������.
		/*!
		*
		*/
		Resource();

	public:
		//! ���������� �������. �������� �������� ��������, ��������� � �.�.
		/*!
		* return ���� ���������� ������ �������
		*/
		virtual bool compile();

		//! ������������ �������.
		/*!
		*
		*/
		virtual void release();

		//! �������� �������� �� ������ ����������������.
		/*!
		* return ���� �������������
		*/
		virtual bool isCompile();

	protected:
		virtual bool _compile();
		virtual void _release();

	protected:
		bool m_compile;
	};
}