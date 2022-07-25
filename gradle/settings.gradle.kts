fun getBooleanProperty(name: String, d: Boolean): Boolean {
    if( extra.has(name) == false ) {
        return d;
    }

    try {
        val value = extra[name].toString()

        if( value.toInt() == 0) {
            return false
        }

        return true
    } catch (e: Exception) {
    }

    return d;
}

fun includePlugin(name: String, d: Boolean, path: String) {
    if( getBooleanProperty(name, d) == false ) {
        println("[-] Exclude plugin: $path")

        return;
    }

    println("[+] Include plugin: $path")

    include(path)
}

println("[+] Include :app")
include(":app")

var ANDROID_APP_ENABLE_DELIVERY_PACKAGE = getBooleanProperty("ANDROID_APP_ENABLE_DELIVERY_PACKAGE", false)

println("ANDROID_APP_ENABLE_DELIVERY_PACKAGE: $ANDROID_APP_ENABLE_DELIVERY_PACKAGE")

if( ANDROID_APP_ENABLE_DELIVERY_PACKAGE == true ) {
    if( extra.has("ANDROID_APP_DELIVERY_PACKAGE_NAME") == false ) {
        throw kotlin.Exception("Miss setup app delivery package name [ANDROID_APP_DELIVERY_PACKAGE_NAME]")
    }

    val ANDROID_APP_DELIVERY_PACKAGE_NAME = extra["ANDROID_APP_DELIVERY_PACKAGE_NAME"].toString()

    println("[+] Include delivery: :app:$ANDROID_APP_DELIVERY_PACKAGE_NAME")

    include(":app:$ANDROID_APP_DELIVERY_PACKAGE_NAME")

    if( extra.has("ANDROID_APP_DELIVERY_PACKAGE_EXTRA_PATH") == true ) {
        val ANDROID_APP_DELIVERY_PACKAGE_EXTRA_PATH = extra["ANDROID_APP_DELIVERY_PACKAGE_EXTRA_PATH"].toString()

        println("ANDROID_APP_DELIVERY_PACKAGE_EXTRA_PATH: $ANDROID_APP_DELIVERY_PACKAGE_EXTRA_PATH")

        val f = File(ANDROID_APP_DELIVERY_PACKAGE_EXTRA_PATH)

        if( f.exists() == false ) {
            println("Not exist delivery extra path [ANDROID_APP_DELIVERY_PACKAGE_EXTRA_PATH]: $ANDROID_APP_DELIVERY_PACKAGE_EXTRA_PATH")
            throw kotlin.Exception("Not exist delivery directory: $ANDROID_APP_DELIVERY_PACKAGE_EXTRA_PATH")
        }

        project(":app:$ANDROID_APP_DELIVERY_PACKAGE_NAME").projectDir = File(ANDROID_APP_DELIVERY_PACKAGE_EXTRA_PATH)
    }
}

includePlugin("MENGINE_APP_PLUGIN_MENGINE", true, ":libraries:Mengine")
includePlugin("MENGINE_APP_PLUGIN_OPENAL32", true, ":libraries:OpenAL32")
includePlugin("MENGINE_APP_PLUGIN_SDL2", true, ":libraries:SDL2")

includePlugin("MENGINE_APP_PLUGIN_FIREBASE_ANALYTICS", false,":libraries:FirebaseAnalytics")
includePlugin("MENGINE_APP_PLUGIN_FIREBASE_CRASHLYTICS", false,":libraries:FirebaseCrashlytics")
includePlugin("MENGINE_APP_PLUGIN_APPLOVIN", false, ":libraries:AppLovin")
includePlugin("MENGINE_APP_PLUGIN_SENTRY",false, ":libraries:Sentry")
includePlugin("MENGINE_APP_PLUGIN_FACEBOOK", false,":libraries:Facebook")
includePlugin("MENGINE_APP_PLUGIN_DEVTODEV", false, ":libraries:DevToDev")
includePlugin("MENGINE_APP_PLUGIN_LOCAL_NOTIFICATIONS", false, ":libraries:LocalNotifications")
includePlugin("MENGINE_APP_PLUGIN_GOOGLE_SERVICE", false, ":libraries:GoogleService")
includePlugin("MENGINE_APP_PLUGIN_GOOGLE_BILLING", false, ":libraries:GoogleBilling")
includePlugin("MENGINE_APP_PLUGIN_GOOGLE_GAME_SOCIAL",false, ":libraries:GoogleGameSocial")
includePlugin("MENGINE_APP_PLUGIN_MAR", false, ":libraries:MAR")
includePlugin("MENGINE_APP_PLUGIN_ADJUST", false, ":libraries:Adjust")
includePlugin("MENGINE_APP_PLUGIN_GOOGLE_INAPP_REVIEWS",false, ":libraries:GoogleInAppReviews")

println("Mengine complete settings")
