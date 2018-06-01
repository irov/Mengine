#pragma once

#include "Interface/TimelineInterface.h"

#include "Core/ServiceBase.h"

#include "Config/Vector.h"

namespace Mengine
{
	class TimelineService
		: public ServiceBase<TimelineServiceInterface>
	{
	public:
		TimelineService();
		~TimelineService() override;

	public:
		void beginOffset( float _offset ) override;
		float getOffset() const override;
		void endOffset() override;

	public:
		float getTime() const override;	

	public:
		void tick( float _offset ) override;

	protected:
		float m_time;

		typedef Vector<float> TVectorTime;
		TVectorTime m_offsets;
	};
};
