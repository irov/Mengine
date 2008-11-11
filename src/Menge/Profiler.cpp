#	include "Profiler.h"	
#	include "RenderEngine.h"

namespace Menge
{
	TimerInterface * Profiler::m_clock = 0;
	bool Profiler::m_enabled = false;
	unsigned long int Profiler::m_currentCycleStartMicroseconds = 0;
	unsigned long int Profiler::m_lastCycleDurationMicroseconds = 0;
	TProfileBlockMap Profiler::m_profileBlocks;
	unsigned long int Profiler::m_cycleNumber = 0;
	//////////////////////////////////////////////////////////////////////////
	Profiler::Profiler()
	{}
	//////////////////////////////////////////////////////////////////////////
	Profiler::~Profiler()
	{}
	//////////////////////////////////////////////////////////////////////////
	void Profiler::setTimer(TimerInterface * _clock)
	{
		m_clock = _clock;
	}
	//////////////////////////////////////////////////////////////////////////
	void Profiler::setEnabled(bool _enabled)
	{
		m_enabled = _enabled;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool Profiler::isEnabled()
	{
		return m_enabled;
	}
	//////////////////////////////////////////////////////////////////////////
	void Profiler::init()
	{
		m_enabled = true;
		
		m_clock->reset();

		m_currentCycleStartMicroseconds = m_clock->getMicroseconds();
	}
	//////////////////////////////////////////////////////////////////////////
	void Profiler::destroy()
	{
		endProfilingCycle();

		if (m_enabled == false)
		{
			return;
		}

		for(TProfileBlockMap::iterator it = m_profileBlocks.begin(); it != m_profileBlocks.end();
			++it)
		{
			delete (*it).second;
		}

		m_profileBlocks.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Profiler::beginBlock(const Menge::String& name)
	{
		if (m_enabled == false)
		{
			return;
		}

		if (name.empty())
		{
			return;
		}

		ProfileBlock * block = findBlock(name);

		if(block == NULL)
		{
			block = new ProfileBlock();
			m_profileBlocks.insert( std::make_pair(name, block) );
		}

		block->currentBlockStartMicroseconds = m_clock->getMicroseconds();
	}
	//////////////////////////////////////////////////////////////////////////
	void Profiler::endBlock(const Menge::String& name)
	{
		if (m_enabled == false)
		{
			return;
		}

		unsigned long int endTick = m_clock->getMicroseconds();

		ProfileBlock * block = findBlock(name);

		if(block == NULL)
		{
			return;
		}

		unsigned long int blockDuration = endTick - 
			block->currentBlockStartMicroseconds;

		block->currentCycleTotalMicroseconds += blockDuration;
		block->totalMicroseconds += blockDuration;
	}
	//////////////////////////////////////////////////////////////////////////
	ProfileBlock * Profiler::findBlock(const Menge::String & _name)
	{
		TProfileBlockMap::iterator it = m_profileBlocks.find(_name);

		if(it == m_profileBlocks.end())
		{
			return NULL;
		}

		return it->second;
	}
	//////////////////////////////////////////////////////////////////////////
	double Profiler::getBlockTime(const Menge::String& name, 
		BlockTimingMethod method)
	{
		if (m_enabled == false)
		{
			return 0;
		}

		ProfileBlock * block = findBlock(name);

		if(block == NULL)
		{
			return 0;
		}

		double result = 0;

		switch(method)
		{
			case BLOCK_TOTAL_SECONDS:
				result = (double)block->totalMicroseconds * (double)0.000001;
				break;

			case BLOCK_TOTAL_MILLISECONDS:
				result = (double)block->totalMicroseconds * (double)0.001;
				break;

			case BLOCK_TOTAL_MICROSECONDS:
				result = (double)block->totalMicroseconds;
				break;

			case BLOCK_TOTAL_PERCENT:
			{
				double timeSinceInit = (double)m_clock->getMicroseconds();
				if (timeSinceInit <= 0)
				{
					result = 0;
				}
				else
				{
					result = 100.0 * (double)block->totalMicroseconds / 
						timeSinceInit;
				}
				break;
			}

			case BLOCK_CYCLE_SECONDS:
				result = (double)block->lastCycleTotalMicroseconds * 
					(double)0.000001;
				break;

			case BLOCK_CYCLE_MILLISECONDS:
				result = (double)block->lastCycleTotalMicroseconds * 
					(double)0.001;
				break;

			case BLOCK_CYCLE_MICROSECONDS:
				result = (double)block->lastCycleTotalMicroseconds;
				break;

			case BLOCK_CYCLE_PERCENT:
			{
				if (0 == m_lastCycleDurationMicroseconds)
				{
					result = 0;
				}
				else
				{
					result = 100.0 * (double)block->lastCycleTotalMicroseconds / 
						m_lastCycleDurationMicroseconds;
				}
				break;
			}
			default:
				break;
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Profiler::endProfilingCycle()
	{
		if (m_enabled == false)
		{
			return;
		}

		m_lastCycleDurationMicroseconds = m_clock->getMicroseconds() - 
			m_currentCycleStartMicroseconds;

		for (TProfileBlockMap::iterator it = m_profileBlocks.begin(); 
			it != m_profileBlocks.end(); ++it)
		{
			ProfileBlock * block = (*it).second;

			block->lastCycleTotalMicroseconds = 
				block->currentCycleTotalMicroseconds;
			block->currentCycleTotalMicroseconds = 0;
		}

		++m_cycleNumber;
		m_currentCycleStartMicroseconds = m_clock->getMicroseconds();
	}
	//////////////////////////////////////////////////////////////////////////
	Menge::String Profiler::createStatsString(BlockTimingMethod method)
	{
		if (m_enabled == false)
		{
			return "";
		}

		Menge::String s;
		Menge::String suffix;

		switch(method)
		{
			case BLOCK_TOTAL_SECONDS:
				suffix = "s";
				break;
			case BLOCK_TOTAL_MILLISECONDS:
				suffix = "ms";
				break;
			case BLOCK_TOTAL_MICROSECONDS:
				suffix = "us";
				break;
			case BLOCK_TOTAL_PERCENT:
			{
				suffix = "%";
				break;
			}
			case BLOCK_CYCLE_SECONDS:
				suffix = "s";
				break;
			case BLOCK_CYCLE_MILLISECONDS:
				suffix = "ms";
				break;
			case BLOCK_CYCLE_MICROSECONDS:
				suffix = "us";
				break;
			case BLOCK_CYCLE_PERCENT:
			{
				suffix = "%";
				break;
			}
			default:
				break;
		}

		for (TProfileBlockMap::iterator it = m_profileBlocks.begin();
			it != m_profileBlocks.end(); ++it)
		{
			if (it != m_profileBlocks.begin())
			{
				s += "\n";
			}

			char blockTime[64];
			sprintf(blockTime, "%lf", getBlockTime((*it).first, method));

			s += (*it).first;
			s += ": ";
			s += blockTime;
			s += " ";
			s += suffix;
		}

		return s;
	}
	//////////////////////////////////////////////////////////////////////////	
	void Profiler::drawStats()
	{
		if(m_enabled)
		{
			Holder<RenderEngine>::hostage()
				->setRenderArea( mt::vec4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

			Holder<RenderEngine>::hostage()
				->beginLayer2D();

			static TChar m_debugText[128];

			int dipCount = Holder<RenderEngine>::hostage()->getNumDIP();
			std::sprintf( m_debugText, "DIP = %d\n", dipCount );

			mt::vec2f pos(0,20);
			Holder<RenderEngine>::hostage()->renderText( m_debugText, pos, 0xFFFFFFFF );

			pos.y +=20;
			Menge::String str = Profiler::createStatsString(Profiler::BLOCK_CYCLE_PERCENT);
			Holder<RenderEngine>::hostage()->renderText(str, pos, 0xFFFFFFFF);
		
			Holder<RenderEngine>::hostage()
				->endLayer2D();
		}
	}
}