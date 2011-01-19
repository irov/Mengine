#	pragma once

#	include "Entity.h"
#	include "Core/Resolution.h"

#	include "Math/vec4.h"
#	include "Math/polygon.h"

namespace Menge
{
	//! Arrow - курсор. ћожно повесить на курсор HotSpot, и пересечение будет провер€тс€ как хот спот.

    /*! xml - файл имеет следующую структуру:
	 *	<Node Name = "им€_ноды" Type = "TextField">
     *      <ClickOffset Value = "значение_смещени€_курсора"/>
	 *	</Node>
	*/

	class Arrow
		: public Entity
	{
	public:
		Arrow();

	public:
		void hide( bool _value ) override;

	public:
		Scene * getScene() override;

	public:
		void setOffsetClick( const mt::vec2f & _offsetClick );
		const mt::vec2f & getOffsetClick() const;

		void setPolygon( const mt::polygon & _polygon );
		const mt::polygon & getPolygon() const;

		void setWindow( const Resolution & _window );
		void setCursorMode( bool _mode );

	public:
		void loader( BinParser * _parser ) override;

	protected:
		void addPoint_( const mt::vec2f & _v );

	public:
		void onMouseMove( float _dx, float _dy );

	public:
		void onAppMouseLeave();
		void onAppMouseEnter();

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
