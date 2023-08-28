MENGINE_PROJECT(AppleHelpshiftPlugin)

ADD_FILTER(
src
    AppleHelpshiftPlugin.h
    AppleHelpshiftPlugin.mm

    AppleHelpshiftInterface.h

    AppleHelpshiftService.h
    AppleHelpshiftService.mm
    
    AppleHelpshiftDelegate.h
    AppleHelpshiftDelegate.mm
)

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_HELPSHIFT)

ADD_MENGINE_COCOAPOD("HelpshiftX" "NO-GIT" "10.2.1")
