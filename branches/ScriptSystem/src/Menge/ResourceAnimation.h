#	pragma once

#	include "ResourceReference.h"

#	include <vector>

namespace Menge
{
	//! ResourceAnimation - ������-���� ������� �������� ������ ��� (������, ��������).

    /** xml - ���� ����� ��������� ���������:
	 *<Resource Name = "���_�������" Type = "ResourceAnimation" >
	 *	<Sequences>
	 *		<Sequence Index = "������_�����������0" Delay = "��������0" />
	 *			...
	 *		<Sequence Index = "������_�����������N" Delay = "��������N" />
	 * 	</Sequences>
	 *</Resource>
	*/

	class ResourceAnimation
		: public ResourceReference
	{
		 RESOURCE_DECLARE( ResourceAnimation )

	public:
		//! �����������.
		/*!
		\param _name ��� �������.
		*/
		ResourceAnimation();

	public:

		//! ���������� ���������� �����������
		/*!
		\return ���������� �����������
		*/
		virtual std::size_t getSequenceCount() const;

		//! ���������� �������� ��� _sequence �����
		/*!
		\param _sequence ������ �����
		\return ��������
		*/
		virtual float getSequenceDelay( std::size_t _sequence ) const;

		
		//! ���������� ������ �����������
		/*!
		\param _sequence ������ �����
		\return ������ �����������
		*/
		virtual std::size_t getSequenceIndex( std::size_t _sequence ) const;

	public:
		void loader( XmlElement * _xml );
		void loaderSequences_( XmlElement * _xml );

	protected:
		bool _compile() override;

	private:
		struct Sequence
		{
			float delay;
			std::size_t index;
		};

		typedef std::vector<Sequence> TVectorSequence;
		TVectorSequence m_vectorSequence;
	};
}
