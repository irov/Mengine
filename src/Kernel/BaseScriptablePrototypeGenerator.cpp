#include "BaseScriptablePrototypeGenerator.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/NotificatorInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseScriptablePrototypeGenerator::BaseScriptablePrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseScriptablePrototypeGenerator::~BaseScriptablePrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ScriptWrapperInterfacePtr & BaseScriptablePrototypeGenerator::getScriptWrapper() const
    {
        return m_scriptWrapper;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseScriptablePrototypeGenerator::registerScriptWrapperObserver()
    {
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING_END, this, [this]()
        {
            const ConstString & prototype = this->getPrototype();

            ScriptWrapperInterfacePtr scriptWrapper = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), prototype );

            if( scriptWrapper == nullptr )
            {
                LOGGER_WARNING( "Scriptable category '%s' prototype '%s' generetor not found ClassWrapping"
                    , this->getCategory().c_str()
                    , prototype.c_str()
                );

                return;
            }

            m_scriptWrapper = scriptWrapper;
        }, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseScriptablePrototypeGenerator::unregisterScriptWrapperObserver()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING_END );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseScriptablePrototypeGenerator::setupScriptable( const ScriptablePtr & _scriptable )
    {
        _scriptable->setScriptWrapper( m_scriptWrapper );
    }
}