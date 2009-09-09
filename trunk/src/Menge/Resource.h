#	pragma once

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
		inline bool isCompile() const;

	protected:
		virtual bool _compile();
		virtual void _release();

	protected:
		bool m_compile;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Resource::isCompile() const
	{
		return m_compile;
	}
}
