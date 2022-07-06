fun getBooleanProperty(name: String, d: Boolean): Boolean {
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

    if( extra.has("ANDROID_APP_DELIVERY_PACKAGE_EXT>RA_PATH") == true ) {
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

includePlugin("ANDROID_APP_ENABLE_MENGINE", true, ":libraries:Mengine")
includePlugin("ANDROID_APP_ENABLE_OPENAL32", true, ":libraries:OpenAL32")
includePlugin("ANDROID_APP_ENABLE_SDL2", true, ":libraries:SDL2")

includePlugin("ANDROID_APP_ENABLE_FIREBASE_ANALYTICS", true,":libraries:FirebaseAnalytics")
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
includePlugin("ANDROID_APP_ENABLE_GOOGLE_INAPP_REVIEWS",true, ":libraries:GoogleInAppReviews")

println("Mengine complete settings")
