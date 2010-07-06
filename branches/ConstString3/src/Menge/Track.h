#	pragma once

#	include "Node.h"

#	include "TrackChain.h"

#	include "Math/vec3.h"

#	include <list>


namespace Menge
{
	class TrackProxy;

	typedef std::list<TrackChain> TListTrackChain;

	//! Track - путь по которому может двигаться объект, наследуемый от Node.
	/*! 
	*
	*/

	/*! xml - файл имеет следующую структуру:
	*	<Node Name = "имя_ноды" Type = "Track">
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
	public:
		Track();

	public:
		//! Добавить точку.
		/*!
		\param _point точка.
		*/
		void addPoint( const mt::vec2f & _point );

		//! Скорость движения по треку.
		/*!
		\param _speed скорость.
		*/
		void setSpeedMove( float _speed );

		//! Возвращает скорость движения по треку
		/*!
		\return скороть
		*/
		float getSpeedMove()const;

		//! Запуск движения.
		/*!
		*/
		void start();

		//! Остановка движения.
		/*!
		*/
		void stop();

		//! Возвращает признак движения по треку.
		/*!
		*/
		bool isMoved()const;

	public:
		void loader( BinParser * _parser ) override;

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
