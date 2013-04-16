#	include "EmitterDescription.h"

namespace Menge
{
	EmitterDescription::EmitterDescription()
		: m_leftBorder(0.0f)
		, m_rightBorder(0.0f)
		, m_emitterId(0)
		, m_sizes(0,0,0,0)
		, m_duration(0.0f)
		, m_relative(true)
	{
	}
	///////////////////////////////////////////////////////////
	void EmitterDescription::setName( const String& _name )
	{
		m_name = _name;
	}
	///////////////////////////////////////////////////////////
	void EmitterDescription::setDuration( float _duration )
	{
		m_duration = _duration;
	}
	///////////////////////////////////////////////////////////
	void EmitterDescription::setLeftBorder( float _interval1 )
	{
		m_leftBorder = _interval1;
	}
	///////////////////////////////////////////////////////////
	void EmitterDescription::setRightBorder( float _interval2 )
	{
		m_rightBorder = _interval2;
	}
	///////////////////////////////////////////////////////////
	void EmitterDescription::setEmitterId( size_t _id )
	{
		m_emitterId = _id;
	}
	///////////////////////////////////////////////////////////
	void EmitterDescription::setSize( const Rect & _rect )
	{
		m_sizes = _rect;
	}
	///////////////////////////////////////////////////////////
	float EmitterDescription::getDuration() const
	{
		return m_duration;
	}
	///////////////////////////////////////////////////////////
	float EmitterDescription::getLeftBorder() const
	{
		return m_leftBorder;
	}
	///////////////////////////////////////////////////////////
	float EmitterDescription::getRightBorder() const
	{
		return m_rightBorder;
	}
	///////////////////////////////////////////////////////////
	const String & EmitterDescription::getName() const
	{
		return m_name;
	}
	///////////////////////////////////////////////////////////
	const Rect& EmitterDescription::getSize() const
	{
		return m_sizes;
	}
	///////////////////////////////////////////////////////////
	int EmitterDescription::getEmitterId() const
	{
		return m_emitterId;
	}
	///////////////////////////////////////////////////////////
	void EmitterDescription::setRelative( bool _relative )
	{
		m_relative = _relative;
	}
	///////////////////////////////////////////////////////////
	bool EmitterDescription::getRelative() const 
	{
		return m_relative;
	}
	///////////////////////////////////////////////////////////
}