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

	struct AnimationSequence
	{
		float delay;
		std::size_t index;
	};

	typedef std::vector<AnimationSequence> TVectorAnimationSequence;

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
		std::size_t getSequenceCount() const;

		//! ���������� �������� ��� _sequence �����
		/*!
		\param _sequence ������ �����
		\return ��������
		*/
		float getSequenceDelay( std::size_t _sequence ) const;

		
		//! ���������� ������ �����������
		/*!
		\param _sequence ������ �����
		\return ������ �����������
		*/
		std::size_t getSequenceIndex( std::size_t _sequence ) const;

	public:
		void setSequences( const TVectorAnimationSequence & _sequence );
		const TVectorAnimationSequence & getSequences() const;

	public:
		void loader( BinParser * _parser ) override;
		void loaderSequences_( BinParser * _parser );

	protected:
		bool _compile() override;

	private:
		
		TVectorAnimationSequence m_sequence;
	};
}
