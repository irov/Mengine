#	pragma once

#	include "Interface/TimelineInterface.h"

#	include "stdex/stl_vector.h"

namespace Menge
{
	class TimelineService
		: public ServiceBase<TimelineServiceInterface>
	{
	public:
		TimelineService();
		~TimelineService();

	public:
		void beginOffset( float _offset ) override;
		float getOffset() const override;
		void endOffset() override;

	public:
		float getTime() const override;	

	public:
		void update( float _offset ) override;

	protected:
		float m_time;

		typedef stdex::vector<float> TVectorTime;
		TVectorTime m_offsets;
	};
};
