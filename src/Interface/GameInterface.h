#   pragma once

#   include "Interface/ServiceInterface.h"

namespace Menge
{
    class Node;

    class GameServiceInterface
        : virtual public ServiceInterface
    {
        SERVICE_DECLARE("GameService")

    public:
        virtual bool render() = 0;

    public:
        virtual float getTimingFactor() const = 0;
        virtual void setTimingFactor( float _timingFactor ) = 0;

    public:
        virtual const WString & getParam( const WString & _paramName ) = 0;
        virtual bool hasParam( const WString & _paramName ) const = 0;

    public:
        virtual const ConstString & getLanguagePack() const = 0;
    };

#   define GAME_SERVICE( serviceProvider )\
    (Menge::getService<Menge::GameServiceInterface>(serviceProvider))
}