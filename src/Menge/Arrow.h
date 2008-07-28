#	pragma once

#	include "Node.h"
#	include "Math/vec4.h"

namespace Menge
{
	class Sprite;

	class HotSpot;

	//! Arrow - курсор. ћожно повесить на курсор HotSpot, и пересечение будет провер€тс€ как хот спот.

    /*! xml - файл имеет следующую структуру:
	 *	<Node Name = "им€_ноды" Type = "TextField">
     *      <ClickOffset Value = "значение_смещени€_курсора"/>
	 *	</Node>
	*/

	class Arrow
		: public Node
	{
		OBJECT_DECLARE(Arrow)

	public:
		Arrow();

		void hide( bool _value );

	public:
		void setOffsetClick( const mt::vec2f & _offsetClick );
		const mt::vec2f & getOffsetClick() const;

		void addHotSpot( HotSpot * _hotspot );
		HotSpot * getCurrentHotSpot() const;

	public:
		void loader( XmlElement * _xml ) override;
		void loaderArrow_( XmlElement * _xml );

		void onMouseMove( float _dx, float _dy );
		void onMouseLeave();
		void onMouseEnter();

		mt::vec2f getScreenPosition() override;
	protected:
		void _update( float _timing ) override;
		bool _activate() override;
		bool _compile() override;

	protected:
		mt::vec2f m_offsetClick;
		mt::vec4f m_window;
		HotSpot * m_currentHotSpot;
		bool m_hided;
	};
}