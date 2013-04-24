#	pragma once

#   include "Interface/MovieKeyFrameInterface.h"

#	include "MovieFramePack.h"

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

#	include <vector>

namespace Menge
{
	class MovieKeyFrameManager
        : public MovieKeyFrameServiceInterface
	{
	public:
		MovieKeyFrameManager();
		~MovieKeyFrameManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		MovieFramePackInterface * getMovieFramePak( const ConstString & _pak, const FilePath & _path ) override;
		void releaseMovieFramePak( MovieFramePackInterface * _framePak ) override;

	protected:
		MovieFramePackInterface * createMovieFramePak_( const ConstString & _pak, const FilePath & _path );

    protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}