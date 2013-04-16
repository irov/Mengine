#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

#   include "Math/vec3.h"

#   include <vector>

namespace Menge
{
    struct MovieFrameSource
    {
        mt::vec3f anchorPoint;
        mt::vec3f position;
        mt::vec3f rotation;
        mt::vec3f scale;
        float opacity;
        float volume;
    };

    typedef std::vector<MovieFrameSource> TVectorMovieFrameSource;

    struct MovieLayerFrame
    {	
        size_t count;
        TVectorMovieFrameSource frames;

        MovieFrameSource source;
        bool immutable;
    };

    typedef std::vector<MovieLayerFrame> TVectorMovieFrameLayer;

    class MovieFramePackInterface
    {
    public:
        virtual const MovieLayerFrame & getLayer( size_t _layerIndex ) const = 0;

    public:
        virtual bool getLayerFrame( size_t _layerIndex, size_t _frameIndex, MovieFrameSource & _frame ) const = 0;
    };

    class MovieKeyFrameServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("MovieKeyFrameService")

    public:
        virtual MovieFramePackInterface * getMovieFramePak( const ConstString & _pak, const FilePath & _path ) = 0;
        virtual void releaseMovieFramePak( MovieFramePackInterface * _framePak ) = 0;
    };

#   define MOVIEKEYFRAME_SERVICE( serviceProvider )\
    (Menge::getService<Menge::MovieKeyFrameServiceInterface>(serviceProvider))
}