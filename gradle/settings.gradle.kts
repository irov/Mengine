fun getBooleanProperty(name: String, d: Boolean): Boolean {
    try {
        val value = extra[name].toString()

        if (value.toInt() == 0) {
            return false
        }

        return true
    } catch (e: Exception) {
    }

    return d;
}

fun includePlugin(name: String, d: Boolean, path: String) {
    if (getBooleanProperty(name, d) == false) {
        println("[-] Exclude plugin: " + path)

        return;
    }

    println("[+] Include plugin: " + path)

    include(path)
}

println("[+] Include :app")
include(":app")

if (getBooleanProperty("ANDROID_APP_ENABLE_DELIVERY_DIR", false) == true) {
    try {
        var ANDROID_APP_DELIVERY_DIR = extra["ANDROID_APP_DELIVERY_DIR"].toString()
        val pwd = System.getProperty("user.dir")
        if (File(pwd + ANDROID_APP_DELIVERY_DIR).exists()) {
            ANDROID_APP_DELIVERY_DIR = pwd + ANDROID_APP_DELIVERY_DIR
        } else if (!File(ANDROID_APP_DELIVERY_DIR).exists()) {
            throw java.lang.Exception("no fin delivery app")
        }

        val delivery_app_id = extra["ANDROID_DELIVERY_APP_ID"].toString()
        println("[+] Include $delivery_app_id -> $ANDROID_APP_DELIVERY_DIR")

        include(":app:$delivery_app_id")
        project(":app:$delivery_app_id").projectDir = File(ANDROID_APP_DELIVERY_DIR)
    } catch (e: Exception) {
        println("Miss setup app delivery directory [ANDROID_DELIVERY_ASSETS_SRC_DIR]")
    }
}

includePlugin("ANDROID_APP_ENABLE_MENGINE", true, ":libraries:Mengine")
includePlugin("ANDROID_APP_ENABLE_OPENAL32", true, ":libraries:OpenAL32")
includePlugin("ANDROID_APP_ENABLE_SDL2", true, ":libraries:SDL2")

includePlugin("ANDROID_APP_ENABLE_FIREBASE_ANALYTICS", true, ":libraries:FirebaseAnalytics")
includePlugin("ANDROID_APP_ENABLE_FIREBASE_CRASHLYTICS", true, ":libraries:FirebaseCrashlytics")
includePlugin("ANDROID_APP_ENABLE_APPLOVIN", true, ":libraries:Applovin")
includePlugin("ANDROID_APP_ENABLE_SENTRY", true, ":libraries:Sentry")
includePlugin("ANDROID_APP_ENABLE_FACEBOOK", true, ":libraries:Facebook")
includePlugin("ANDROID_APP_ENABLE_DEVTODEV", true, ":libraries:DevToDev")
includePlugin("ANDROID_APP_ENABLE_LOCAL_NOTIFICATIONS", true, ":libraries:LocalNotifications")
includePlugin("ANDROID_APP_ENABLE_GOOGLE_SERVICE", true, ":libraries:GoogleService")
includePlugin("ANDROID_APP_ENABLE_GOOGLE_BILLING", true, ":libraries:GoogleBilling")
includePlugin("ANDROID_APP_ENABLE_GOOGLE_GAME_SOCIAL", true, ":libraries:GoogleGameSocial")
includePlugin("ANDROID_APP_ENABLE_MAR", false, ":libraries:MAR")
includePlugin("ANDROID_APP_ENABLE_ADJUST", false, ":libraries:Adjust")
includePlugin("ANDROID_APP_ENABLE_GOOGLE_INAPP_REVIEWS", true, ":libraries:GoogleInAppReviews")

println("Complete settings")
