fun getBooleanProperty(name: String, d: Boolean): Boolean {
    val value = System.getProperty(name)

    if( value.isNullOrBlank() == true ) {
        return d
    }

    if( value.toInt() == 0) {
        return false
    }

    return true
}

fun includePlugin(name: String, d: Boolean, path: String) {
    if( getBooleanProperty(name, d) == false ) {
        return;
    }

    include (path)
}

include (":app")

if( getBooleanProperty("ANDROID_APP_ENABLE_DELIVERY_DIR", false) == true ) {
    val ANDROID_APP_DELIVERY_DIR = System.getProperty("ANDROID_APP_DELIVERY_DIR")

    include(":app:" + ANDROID_APP_DELIVERY_DIR)
}

include (":libraries:Mengine")

includePlugin("ANDROID_APP_ENABLE_FIREBASE_ANALYTICS", true,":libraries:FirebaseAnalytics");
includePlugin("ANDROID_APP_ENABLE_FIREBASE_CRASHLYTICS", true,":libraries:FirebaseCrashlytics")
includePlugin("ANDROID_APP_ENABLE_APPLOVIN", true, ":libraries:Applovin")
includePlugin("ANDROID_APP_ENABLE_SENTRY",true, ":libraries:Sentry")
includePlugin("ANDROID_APP_ENABLE_FACEBOOK", true,":libraries:Facebook")
includePlugin("ANDROID_APP_ENABLE_DEVTODEV", true, ":libraries:DevToDev")
includePlugin("ANDROID_APP_ENABLE_LOCAL_NOTIFICATIONS", true, ":libraries:LocalNotifications")
includePlugin("ANDROID_APP_ENABLE_GOOGLE_SERVICE", true, ":libraries:GoogleService")
includePlugin("ANDROID_APP_ENABLE_GOOGLE_BILLING", true, ":libraries:GoogleBilling")
includePlugin("ANDROID_APP_ENABLE_GOOGLE_GAME_SOCIAL",true, ":libraries:GoogleGameSocial")
includePlugin("ANDROID_APP_ENABLE_MAR", false, ":libraries:MAR")
includePlugin("ANDROID_APP_ENABLE_ADJUST", false, ":libraries:Adjust")

include (":libraries:OpenAL32")
include (":libraries:SDL2")