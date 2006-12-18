#	pragma once

#	include <string>

namespace	Menge
{
	class MessageSpot 
	{
	public:
		MessageSpot();
		MessageSpot(int _id, const std::string& _message, const std::string& _sname, float _width);
		~MessageSpot();

		void setText(const std::string& _text);
		std::string& getText();

		void setSoundName(const std::string& _name);
		std::string& getSoundName();

		void setWidth(float _width);
		float	getWidth() const;
	private:
		int	m_id;
		float	m_width;
		std::string	m_message;
		std::string	m_soundName;
	};
};