#include "PlayerGlobalAffectorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PlayerGlobalAffectorable::PlayerGlobalAffectorable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PlayerGlobalAffectorable::~PlayerGlobalAffectorable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const AffectorHubProviderInterface * PlayerGlobalAffectorable::getAffectorHubProvider() const
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    EUpdateMode PlayerGlobalAffectorable::getAffectorableUpdatableMode() const
    {
        return EUM_SERVICE_BEFORE;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t PlayerGlobalAffectorable::getAffectorableUpdatableLeafDeep() const
    {
        return 0U;
    }
    //////////////////////////////////////////////////////////////////////////
}