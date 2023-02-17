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
        val red = "\u001b[31m"
        val reset = "\u001b[0m"
        println(red + "[-] Exclude plugin: $path" + reset)

        return;
    }

    val green = "\u001b[32m"
    val reset = "\u001b[0m"
    println(green + "[+] Include plugin: $path" + reset)

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


/*****************************************************************************
/ * - MENGINE_APP_PLUGIN_FIREBASE [https://firebase.google.com]
/ * - MENGINE_APP_PLUGIN_FIREBASE_ANALYTICS [https://firebase.google.com/docs/analytics]
/ * - MENGINE_APP_PLUGIN_FIREBASE_CRASHLYTICS [https://firebase.google.com/docs/crashlytics]
/ * - MENGINE_APP_PLUGIN_FIREBASE_MESSAGING [https://firebase.google.com/docs/cloud-messaging]
/ * - MENGINE_APP_PLUGIN_FIREBASE_REMOTECONFIG [https://firebase.google.com/docs/remote-config]
/ * - MENGINE_APP_PLUGIN_APPLOVIN [https://www.applovin.com]
/ * - MENGINE_APP_PLUGIN_SENTRY [https://sentry.io]
/ * - MENGINE_APP_PLUGIN_FACEBOOK [https://developers.facebook.com]
/ * - MENGINE_APP_PLUGIN_DEVTODEV [https://www.devtodev.com]
/ * - MENGINE_APP_PLUGIN_GOOGLE_SERVICE [https://developers.google.com]
/ * - MENGINE_APP_PLUGIN_GOOGLE_PLAY_BILLING [https://developer.android.com/google/play/billing]
/ * - MENGINE_APP_PLUGIN_GOOGLE_GAME_SOCIAL [https://developers.google.com/games/services]
/ * - MENGINE_APP_PLUGIN_GOOGLE_INAPP_REVIEWS [https://developer.android.com/guide/playcore/in-app-review]
/ * - MENGINE_APP_PLUGIN_GOOGLE_CONSENT [https://developers.google.com]
/ * - MENGINE_APP_PLUGIN_MAR [http://doc.martianyun.com/en]
/ * - MENGINE_APP_PLUGIN_ADJUST [https://www.adjust.com]
/ * - MENGINE_APP_PLUGIN_HELPSHIFT [https://www.helpshift.com]
/ * - MENGINE_APP_PLUGIN_ONESIGNAL [https://onesignal.com]
*****************************************************************************/

includePlugin("MENGINE_APP_PLUGIN_SPLASHSCREEN", false, ":plugins:SplashScreen")
includePlugin("MENGINE_APP_PLUGIN_LOCAL_NOTIFICATIONS", false, ":plugins:LocalNotifications")
includePlugin("MENGINE_APP_PLUGIN_ADVERTISING", false, ":plugins:Advertising")
includePlugin("MENGINE_APP_PLUGIN_GOOGLE_SERVICE", false, ":plugins:GoogleService")
includePlugin("MENGINE_APP_PLUGIN_GOOGLE_PLAY_BILLING", false, ":plugins:GooglePlayBilling")
includePlugin("MENGINE_APP_PLUGIN_GOOGLE_GAME_SOCIAL", false, ":plugins:GoogleGameSocial")
includePlugin("MENGINE_APP_PLUGIN_GOOGLE_INAPP_REVIEWS", false, ":plugins:GoogleInAppReviews")
includePlugin("MENGINE_APP_PLUGIN_GOOGLE_CONSENT", false, ":plugins:GoogleConsent")
includePlugin("MENGINE_APP_PLUGIN_FIREBASE", false,":plugins:Firebase")
includePlugin("MENGINE_APP_PLUGIN_FIREBASE_ANALYTICS", false,":plugins:FirebaseAnalytics")
includePlugin("MENGINE_APP_PLUGIN_FIREBASE_CRASHLYTICS", false,":plugins:FirebaseCrashlytics")
includePlugin("MENGINE_APP_PLUGIN_FIREBASE_MESSAGING", false,":plugins:FirebaseMessaging")
includePlugin("MENGINE_APP_PLUGIN_FIREBASE_REMOTECONFIG", false,":plugins:FirebaseRemoteConfig")
includePlugin("MENGINE_APP_PLUGIN_APPLOVIN", false, ":plugins:AppLovin")
includePlugin("MENGINE_APP_PLUGIN_SENTRY", false, ":plugins:Sentry")
includePlugin("MENGINE_APP_PLUGIN_FACEBOOK", false,":plugins:Facebook")
includePlugin("MENGINE_APP_PLUGIN_DEVTODEV", false, ":plugins:DevToDev")
includePlugin("MENGINE_APP_PLUGIN_MAR", false, ":plugins:MAR")
includePlugin("MENGINE_APP_PLUGIN_ADJUST", false, ":plugins:Adjust")
includePlugin("MENGINE_APP_PLUGIN_HELPSHIFT", false, ":plugins:Helpshift")
includePlugin("MENGINE_APP_PLUGIN_ONESIGNAL", false, ":plugins:OneSignal")
includePlugin("MENGINE_APP_PLUGIN_DEVDEBUGGER", false, ":plugins:DevDebugger")

println("Mengine complete settings")
