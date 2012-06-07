#	pragma once

#	include "Node.h"

#	include "TrackChain.h"

#	include "Math/vec3.h"

#	include <list>


namespace Menge
{
	class TrackProxy;

	typedef std::list<TrackChain> TListTrackChain;

	//! Track - ���� �� �������� ����� ��������� ������, ����������� �� Node.
	/*! 
	*
	*/

	/*! xml - ���� ����� ��������� ���������:
	*	<Node Name = "���_����" Type = "Track">
	*		<Points>
	*			<Point Value = "x0;y0;"/>
	*			 ...
	*			<Point Value = "xN;yN;"/>
	*		</Points>
	*		<SpeedMove Value = "value" /> 
	*	</Node>
	*/

	class Track
		: public Node
	{
		FACTORABLE_DECLARE(Track);

	public:
		Track();

	public:
		//! �������� �����.
		/*!
		\param _point �����.
		*/
		void addPoint( const mt::vec2f & _point );

		//! �������� �������� �� �����.
		/*!
		\param _speed ��������.
		*/
		void setSpeedMove( float _speed );

		//! ���������� �������� �������� �� �����
		/*!
		\return �������
		*/
		float getSpeedMove()const;

		//! ������ ��������.
		/*!
		*/
		void start();

		//! ��������� ��������.
		/*!
		*/
		void stop();

		//! ���������� ������� �������� �� �����.
		/*!
		*/
		bool isMoved()const;

	public:
		void loader(XmlElement * _xml ) override;

	protected:
		void _update( float _timing )  override;

		bool _activate() override;
	private:
		typedef std::list<mt::vec2f> TListPoint;

		TListPoint m_listPoints;
		TListTrackChain m_listChain;

		TListTrackChain::iterator m_currentChain;

		float m_speedMove;
		bool m_isMove;

		friend class TrackProxy;
	};
}
