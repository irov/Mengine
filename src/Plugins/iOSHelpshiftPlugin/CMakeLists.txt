MENGINE_PROJECT(iOSHelpshiftPlugin)

ADD_FILTER(
src
    iOSHelpshiftInterface.h

    iOSHelpshiftDelegate.h
    iOSHelpshiftDelegate.mm

    iOSHelpshiftPlugin.h
    iOSHelpshiftPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_HELPSHIFT)

ADD_MENGINE_COCOAPOD("HelpshiftX" "NO-GIT" "10.5.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSHelpshiftPlugin")