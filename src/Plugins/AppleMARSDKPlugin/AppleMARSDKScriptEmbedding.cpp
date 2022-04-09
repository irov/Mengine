#include "AppleMARSDKScriptEmbedding.h"

#include "AppleMARSDKInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Factory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleMARSDKProvider
            : public AppleMARSDKProviderInterface
        {
        public:
            PythonAppleMARSDKProvider( const pybind::object & _cbUserLogin, const pybind::object & _cbUserLogout, const pybind::object & _cbPayPaid, const pybind::args & _args )
                : m_cbUserLogin( _cbUserLogin )
                , m_cbUserLogout( _cbUserLogout )
                , m_cbPayPaid( _cbPayPaid )
                , m_args( _args )
            {
            }

        protected:
            void onUserLogin() override
            {
                m_cbUserLogin.call_args( m_args );
            }

            void onUserLogout() override
            {
                m_cbUserLogout.call_args( m_args );
            }

            void onPayPaid() override
            {
                m_cbPayPaid.call_args( m_args );
            }

        protected:
            pybind::object m_cbUserLogin;
            pybind::object m_cbUserLogout;
            pybind::object m_cbPayPaid;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonAppleMARSDKProvider, AppleMARSDKProviderInterface> PythonAppleMARSDKProviderPtr;
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleMARSDK_setProvider( const pybind::object & _cbUserLogin, const pybind::object & _cbUserLogout, const pybind::object & _cbPayPaid, const pybind::args & _args )
        {
            PythonAppleMARSDKProviderPtr provider = Helper::makeFactorableUnique<PythonAppleMARSDKProvider>( MENGINE_DOCUMENT_PYBIND, _cbUserLogin, _cbUserLogout, _cbPayPaid, _args );

            APPLE_MARSDK_SERVICE()
                ->setProvider( provider );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleMARSDK_login()
        {
            bool result = APPLE_MARSDK_SERVICE()
                ->login();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleMARSDK_logout()
        {
            bool result = APPLE_MARSDK_SERVICE()
                ->logout();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleMARSDK_switchAccount()
        {
            bool result = APPLE_MARSDK_SERVICE()
                ->switchAccount();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleMARSDK_submitExtendedData( const pybind::dict & _d )
        {
            MARSDKExtraData data;

            data.dataType = _d["dataType"];
            data.opType = _d["opType"];
            data.roleID = _d["roleID"];
            data.roleName = _d["roleName"];
            data.roleLevel = _d["roleLevel"];
            data.serverID = _d["serverID"];
            data.serverName = _d["serverName"];
            data.moneyNum = _d["moneyNum"];
            data.roleCreateTime = _d["roleCreateTime"];
            data.roleLevelUpTime = _d["roleLevelUpTime"];

            data.vip = _d["vip"];
            data.roleGender = _d["roleGender"];
            data.professionID = _d["professionID"];
            data.professionName = _d["professionName"];
            data.power = _d["power"];
            data.partyID = _d["partyID"];
            data.partyName = _d["partyName"];
            data.partyMasterID = _d["partyMasterID"];
            data.partyMasterName = _d["partyMasterName"];

            data.serverId = _d["serverId"];

            APPLE_MARSDK_SERVICE()
                ->submitExtendedData( data );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleMARSDK_submitPayementData( const pybind::dict & _d )
        {
            MARSDKProductInfo info;

            info.orderID = _d["orderID"];
            info.productId = _d["productId"];
            info.productName = _d["productName"];
            info.productDesc = _d["productDesc"];

            info.price = _d["price"];
            info.buyNum = _d["buyNum"];
            info.coinNum = _d["coinNum"];

            info.roleId = _d["roleId"];
            info.roleName = _d["roleName"];
            info.roleLevel = _d["roleLevel"];
            info.vip = _d["vip"];
            info.serverId = _d["serverId"];
            info.serverName = _d["serverName"];
            info.notifyUrl = _d["notifyUrl"];

            APPLE_MARSDK_SERVICE()
                ->submitPayementData( info );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleMARSDKScriptEmbedding::AppleMARSDKScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleMARSDKScriptEmbedding::~AppleMARSDKScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleMARSDK", true );

        pybind::enum_<EMARSDKDataType>( _kernel, "MARSDKDataType" )
            .def( "TYPE_SELECT_SERVER", TYPE_SELECT_SERVER )
            .def( "TYPE_CREATE_ROLE", TYPE_CREATE_ROLE )
            .def( "TYPE_ENTER_GAME", TYPE_ENTER_GAME )
            .def( "TYPE_LEVEL_UP", TYPE_LEVEL_UP )
            .def( "TYPE_EXIT_GAME", TYPE_EXIT_GAME )
            ;

        pybind::def_function_args( _kernel, "appleMARSDKSetProvider", &Detail::s_AppleMARSDK_setProvider );
        pybind::def_function( _kernel, "appleMARSDKLogin", &Detail::s_AppleMARSDK_login );
        pybind::def_function( _kernel, "appleMARSDKLogout", &Detail::s_AppleMARSDK_logout );
        pybind::def_function( _kernel, "appleMARSDKSwitchAccount", &Detail::s_AppleMARSDK_switchAccount );
        pybind::def_function( _kernel, "appleMARSDKSubmitExtendedData", &Detail::s_AppleMARSDK_submitExtendedData );
        pybind::def_function( _kernel, "appleMARSDKSubmitPayementData", &Detail::s_AppleMARSDK_submitPayementData );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleMARSDKSetProvider", nullptr );
        _kernel->remove_from_module( "appleMARSDKLogin", nullptr );
        _kernel->remove_from_module( "appleMARSDKLogout", nullptr );
        _kernel->remove_from_module( "appleMARSDKSwitchAccount", nullptr );
        _kernel->remove_from_module( "appleMARSDKSubmitExtendedData", nullptr );
        _kernel->remove_from_module( "appleMARSDKSubmitPayementData", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

