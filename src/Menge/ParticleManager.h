
class	ParticleManager
{
public:
	ParticleManager(){};
	~ParticleManager(){};

	/*void	loadEmitters(const std::string& _filename)
	{
		std::string emitterFileName;

		XML_PARSE_FILE_EX(_filename)
		{
			XML_CHECK_NODE_FOR_EACH("Emitters")
			{
				XML_CHECK_VALUE_NODE("EmitterFile","File",emitterFileName)
				{
					loadEmitterFromFile(emitterFileName);
				}
			}
		}
	}

	void	loadEmitterFromFile()
	{

	}*/

private:
	std::list<ParticleAffector*>	m_affectors;
	std::list<ParticleEmitter*>		m_emitters;
};