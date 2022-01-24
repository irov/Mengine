#include "AppleGameCenterScriptEmbedding.h"

#include "AppleGameCenterInterface.h"

#include "Interface/SoundServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/AffectorCallbackInterface.h"
#include "Kernel/Factory.h"
#include "Kernel/AffectorHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

namespace Mengine
{    
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        class AppleGameCenterScriptMethod
            : public Factorable
        {
        public:
            AppleGameCenterScriptMethod()
            {
            }

            ~AppleGameCenterScriptMethod() override
            {
            }

        public:
            bool initialize()
            {
                return true;
            }

            void finalize()
            {
            }

        public:

        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<AppleGameCenterScriptMethod> AppleGameCenterScriptMethodPtr;
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleGameCenterScriptEmbedding::AppleGameCenterScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleGameCenterScriptEmbedding::~AppleGameCenterScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        AppleGameCenterScriptMethodPtr scriptMethod = Helper::makeFactorableUnique<AppleGameCenterScriptMethod>( MENGINE_DOCUMENT_FACTORABLE );

        if( scriptMethod->initialize() == false )
        {
            return false;
        }

        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleGameCenter", true );

        m_implement = scriptMethod;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGameCenterScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        AppleGameCenterScriptMethodPtr scriptMethod = m_implement;
        scriptMethod->finalize();

        m_implement = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}

