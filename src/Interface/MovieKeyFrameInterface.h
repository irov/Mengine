#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Config/Floats.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

#   include "Factory/Factorable.h"

#   include "Math/vec3.h"

#   include <vector>

namespace Menge
{
    const size_t INVALID_MASK = (size_t)-1;

    struct MovieFrameSource
    {
        mt::vec3f anchorPoint;
        mt::vec3f position;
        mt::vec3f rotation;
        mt::vec3f scale;
        float opacity;
        float volume;
        size_t mask;
    };

    typedef std::vector<MovieFrameSource> TVectorMovieFrameSource;

    struct MovieLayerFrame
    {
        TVectorMovieFrameSource frames;

        MovieFrameSource source;
        bool immutable;
    };

    struct MovieLayerTimeRemap
    {
        Floats times;
    };

    class MovieFramePackInterface
        : public Factorable
    {
    public:
        virtual bool hasLayer( size_t _layerIndex ) const = 0;
        virtual const MovieLayerFrame & getLayer( size_t _layerIndex ) const = 0;

    public:
        virtual bool getLayerFrame( size_t _layerIndex, size_t _frameIndex, MovieFrameSource & _frame ) const = 0;

    public:
        virtual bool getLayerTimeRemap( size_t _layerIndex, size_t _frameIndex, float & _time ) const = 0;
    };

    class MovieKeyFrameServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("MovieKeyFrameService")

    public:
        virtual MovieFramePackInterface * getMovieFramePak( const ConstString & _pak, const FilePath & _path ) = 0;
    };

#   define MOVIEKEYFRAME_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::MovieKeyFrameServiceInterface>(serviceProvider))
}