#	pragma once

#	include "SceneNode2D.h"

namespace Menge
{
	class Sprite;

	class HotSpot;

	//! Arrow - ������. ����� �������� �� ������ HotSpot, � ����������� ����� ���������� ��� ��� ����.

    /*! xml - ���� ����� ��������� ���������:
	 *	<Node Name = "���_����" Type = "TextField">
     *      <ClickOffset Value = "��������_��������_�������"/>
	 *	</Node>
	*/

	class Arrow
		: public SceneNode2D
	{
		OBJECT_DECLARE(Arrow)

	public:
		Arrow();

	public:
		void setOffsetClick( const mt::vec2f & _offsetClick );
		const mt::vec2f & getOffsetClick() const;

		void addHotSpot( HotSpot * _hotspot );
		HotSpot * getCurrentHotSpot() const;

	public:
		void loader( XmlElement * _xml ) override;
		void loaderArrow_( XmlElement * _xml );

		void onMouseMove( int _dx, int _dy );

	protected:
		bool _renderBegin() override;
		void _renderEnd() override;

	protected:
		void _update( float _timing ) override;
		bool _activate() override;
		void _debugRender() override;
		bool _compile() override;

	protected:
		mt::vec2f m_offsetClick;
		mt::vec2f m_window;
		HotSpot * m_currentHotSpot;
	};
}