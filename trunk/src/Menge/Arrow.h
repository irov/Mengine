#	pragma once

#	include "Node.h"
#	include "Core/Resolution.h"

#	include "Math/vec4.h"
#	include "Math/polygon.h"

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
		: public Node
	{
		FACTORABLE_DECLARE(Arrow)

	public:
		Arrow();

	public:
		void hide( bool _value );

	public:
		Scene * getScene() const override;

	public:
		void setOffsetClick( const mt::vec2f & _offsetClick );
		const mt::vec2f & getOffsetClick() const;

		void setPolygon( const mt::polygon & _polygon );
		const mt::polygon & getPolygon() const;

		void setWindow( const Resolution & _window );
		void setCursorMode( bool _mode );

	public:
		void loader( XmlElement * _xml ) override;
		void loaderArrow_( XmlElement * _xml );
		void addPoint_( const mt::vec2f & _v );

		void onMouseMove( float _dx, float _dy );
		void onMouseLeave();
		void onMouseEnter();

	public:
		mt::vec2f getScreenPosition() override;

	protected:
		void _update( float _timing ) override;
		bool _compile() override;

	protected:
		mt::vec2f m_offsetClick;
		Resolution m_resolution;
		mt::polygon m_polygon;
		bool m_hided;
	};
}
