#	pragma once

#	include "ResourceImpl.h"

#	include <map>

namespace Menge
{
	struct	Message
	{
		int			id;
		float		width;
		std::string	message;
		std::string	soundName;

		Message(int _id, const std::string& _message, const std::string& _sname, float _width)
		: id(_id)
		, width(_width)
		, message(_message)
		, soundName(_sname)
		{}
	};

	class ResourceMessageStorage
		: public ResourceImpl
	{
		RESOURCE_DECLARE(ResourceMessageStorage);
	public:
		ResourceMessageStorage( const std::string & _name );
		const Message *	getMessage(int _id) const;

	public:
		void loader( TiXmlElement * xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		typedef std::map<int,Message*> TMapMessage;
		TMapMessage m_messages;
	};
}