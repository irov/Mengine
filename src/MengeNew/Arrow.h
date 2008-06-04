#	pragma once

#	include "Node.h"

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

	public:
		void setOffsetClick( const mt::vec2f & _offsetClick );
		const mt::vec2f & getOffsetClick() const;

		void addHotSpot( HotSpot * _hotspot );
		HotSpot * getCurrentHotSpot() const;

	public:
		void loader( XmlElement * _xml ) override;
		void loaderArrow_( XmlElement * _xml );

	protected:
		void _update( float _timing ) override;
		bool _activate() override;
		void _debugRender( const Viewport & _viewport ) override;
		bool _compile() override;

	protected:
		mt::vec2f m_offsetClick;

		HotSpot * m_currentHotSpot;
	};
}