#	pragma once
#	include	"Utils/Core/Rect.h"
#	include	"Utils/Core/String.h"
#	include "RenderParticlesInterface.h"

#	define EMITTER_DESCRIPTION_NAME_LENGTH 255

namespace Menge
{
	class EmitterDescription
		: public EmitterDescriptionInterface
	{
	public:
		EmitterDescription( );
		void setName( const String& _name );
		void setLeftBorder( float _interval1 );
		void setRightBorder( float _interval2 );
		void setDuration( float _duration ); 
		void setEmitterId( size_t _id );
		void setSize( const Rect & _rect );

		float getDuration() const override;
		float getLeftBorder() const override;
		float getRightBorder() const override;
		int getEmitterId() const override;
		const String& getName() const override;
		const Rect& getSize() const override;

	protected:
		//ConstString m_name;
		//char m_name[EMITTER_DESCRIPTION_NAME_LENGTH];
		String m_name;
		double m_leftBorder;
		double m_rightBorder;
		double m_duration;
		int m_emitterId;
		Rect m_sizes;
	};
}
