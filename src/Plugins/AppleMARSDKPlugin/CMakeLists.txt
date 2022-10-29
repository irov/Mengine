MENGINE_PROJECT(AppleMARSDKPlugin)

ADD_FILTER(
src
    AppleMARSDKInterface.h

    AppleMARSDKPlugin.h
    AppleMARSDKPlugin.cpp

    AppleMARSDKApplicationDelegate.h
    AppleMARSDKApplicationDelegate.mm

    AppleMARSDKDelegate.h
    AppleMARSDKDelegate.mm

    AppleMARSDKAdRewardedDelegate.h
    AppleMARSDKAdRewardedDelegate.mm

    AppleMARSDKService.h
    AppleMARSDKService.mm
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleMARSDKScriptEmbedding.h
        AppleMARSDKScriptEmbedding.cpp
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_MARSDK)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleMARSDKApplicationDelegate")

ADD_MENGINE_COCOAPOD("MARSDKCore_ios" "NO-GIT" "NO-TAG")
ADD_MENGINE_COCOAPOD("MARSDK_ADVERT" "NO-GIT" "NO-TAG")
ADD_MENGINE_COCOAPOD("MARSDK_UMENG" "NO-GIT" "NO-TAG")
ADD_MENGINE_COCOAPOD("MARSDK_BUGLY" "NO-GIT" "NO-TAG")

if(MENGINE_PLUGIN_APPLE_MARSDK_MEDIATION_ANYTHINK)
	ADD_MENGINE_COCOAPOD("MARSDK_IOSTOPONAD" "NO-GIT" "NO-TAG")
	ADD_MENGINE_COCOAPOD("AnyThinkiOS/AnyThinkKuaiShouAdapter" "NO-GIT" "5.9.95")
	ADD_MENGINE_COCOAPOD("AnyThinkiOS/AnyThinkVungleAdapter" "NO-GIT" "5.9.95")
	ADD_MENGINE_COCOAPOD("AnyThinkiOS/AnyThinkUnityAdsAdapter" "NO-GIT" "5.9.95")
	ADD_MENGINE_COCOAPOD("AnyThinkiOS/AnyThinkTTAdapter" "NO-GIT" "5.9.95")
	ADD_MENGINE_COCOAPOD("AnyThinkiOS/AnyThinkAdmobAdapter" "NO-GIT" "5.9.95")
	ADD_MENGINE_COCOAPOD("AnyThinkiOS/AnyThinkMintegralAdapter" "NO-GIT" "5.9.95")
	ADD_MENGINE_COCOAPOD("AnyThinkiOS/AnyThinkGDTAdapter" "NO-GIT" "5.9.95")
endif()

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
