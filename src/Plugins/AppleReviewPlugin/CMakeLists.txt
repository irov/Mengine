MENGINE_PROJECT(AppleReviewPlugin)

ADD_FILTER(
src
    AppleReviewInterface.h
    AppleReviewService.h
    AppleReviewService.mm

    AppleReviewPlugin.h
    AppleReviewPlugin.cpp

)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_REVIEW)



ADD_APPLE_FRAMEWORK(StoreKit)