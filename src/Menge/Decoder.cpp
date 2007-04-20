#	include "Decoder.h"
#	include "assert.h"

using namespace Menge;
std::map<std::string, Decoder::DecoderComponent*>	Decoder::m_codecs;

void	Decoder::regDecoder(const std::string& _extension, DecoderComponent* _newDecoder)
{
	DecoderMap::const_iterator it = m_codecs.find(_extension);
	if (it != m_codecs.end())
	{
		assert(!"this codec is also registered!");
		return;
	}
	m_codecs.insert(make_pair(_extension,_newDecoder));
}

void	Decoder::unregDecoder(const std::string& _extension)
{
	m_codecs.erase(_extension);
}

DecoderInterface* Decoder::getDecodec(const std::string& _extension)
{
	DecoderMap::iterator it = m_codecs.find(_extension);
	if (it == m_codecs.end())
	{
		assert(!"no codec for this ext found!");
		return NULL;
	}
	return it->second->getDecoder();
}