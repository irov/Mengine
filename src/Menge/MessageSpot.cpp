#include "MessageSpot.h"

namespace	Menge
{
	MessageSpot::MessageSpot()
	: m_id(-1)
	, m_message("")
	, m_soundName("")
	, m_width(-1)
	{}

	MessageSpot::MessageSpot(int _id, const std::string& _message, const std::string& _sname, float _width)
	: m_id(_id)
	, m_message(_message)
	, m_soundName(_sname)
	, m_width(_width)
	{}

	MessageSpot::~MessageSpot()
	{}

	float	MessageSpot::getWidth() const
	{
		return	m_width;
	}

	std::string& MessageSpot::getText()
	{
		return	m_message;
	}

	std::string& MessageSpot::getSoundName()
	{
		return	m_soundName;
	}

	void MessageSpot::setText(const std::string& _text)
	{
		m_message = _text;
	}

	void MessageSpot::setSoundName(const std::string& _name)
	{
		m_soundName = _name;
	}

	void MessageSpot::setWidth(float _width)
	{
		m_width = _width;
	}
};