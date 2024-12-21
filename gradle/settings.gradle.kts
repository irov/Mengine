fun getBooleanProperty(name: String, d: Boolean): Boolean {
    if (extra.has(name) == false) {
        return d
    }

    try {
        val value = extra[name].toString()

        if (value.toInt() == 0) {
            return false
        }

        return true
    } catch (e: Exception) {
    }

    return d
}

fun getStringProperty(name: String, d: String): String {
    if (extra.has(name) == false) {
        return d
    }

    return extra[name].toString()
}

fun includeLibrary(name: String, path: String) {
    if (getBooleanProperty("MENGINE_APP_LIBRARY_ALL", false) == false && getBooleanProperty(name, true) == false) {
        println("\u001b[31m" + "[-] Exclude library: $path" + "\u001b[0m")

        return
    }

    println("\u001b[32m" + "[+] Include library: $path" + "\u001b[0m")

    include(path)
}

fun includePlugin(name: String, path: String) {
    if (getBooleanProperty("MENGINE_APP_PLUGIN_ENABLE_ALL", false) == false && getBooleanProperty(name, false) == false) {
        println("\u001b[31m" + "[-] Exclude plugin: $path" + "\u001b[0m")

        return
    }

    println("\u001b[32m" + "[+] Include plugin: $path" + "\u001b[0m")

    include(path)
}

println("\u001b[32m" + "=== Start configure ===" + "\u001b[0m")

val ANDROID_APP_MAIN_PROJECT = getStringProperty("ANDROID_APP_MAIN_PROJECT", "app");

val fileAppProperties = file("$ANDROID_APP_MAIN_PROJECT/app.properties")

if (fileAppProperties.exists() == false) {
    throw GradleException("properties $ANDROID_APP_MAIN_PROJECT/app.properties is not defined. Please provide a valid path.")
}

val appProperties = java.util.Properties()

fileAppProperties.reader().use { reader -> appProperties.load(reader) }

gradle.beforeProject {
    appProperties.forEach { (key, value) ->
        extensions.extraProperties[key.toString()] = value
    }
}

appProperties.forEach { (key, value) ->
    extra[key.toString()] = value
}

println("\u001b[32m" + "[+] Include :$ANDROID_APP_MAIN_PROJECT" + "\u001b[0m")
include(":$ANDROID_APP_MAIN_PROJECT")

if (extra.has("ANDROID_APP_DELIVERY_PACKAGES") == true) {
    val PACKAGES = extra["ANDROID_APP_DELIVERY_PACKAGES"].toString().split(",")

    for(PACKAGE_DESC in PACKAGES) {
        val PACKAGE_NAME = PACKAGE_DESC.split(";").get(0)
        val PACKAGE_PATH = PACKAGE_DESC.split(";").getOrNull(1)

        if (PACKAGE_PATH == null || PACKAGE_PATH == "NO-PATH") {
            println("\u001b[32m" + "[+] Include delivery: :$ANDROID_APP_MAIN_PROJECT:$PACKAGE_NAME" + "\u001b[0m")
        } else {
            println("\u001b[32m" + "[+] Include delivery: :$ANDROID_APP_MAIN_PROJECT:$PACKAGE_NAME extra path $PACKAGE_PATH" + "\u001b[0m")
        }

        include(":$ANDROID_APP_MAIN_PROJECT:$PACKAGE_NAME")

        if (PACKAGE_PATH != null && PACKAGE_PATH != "NO-PATH") {
            val f = File(PACKAGE_PATH)

            if (f.exists() == false) {
                println("package $PACKAGE_NAME not exist delivery directory: $PACKAGE_PATH")
                throw kotlin.Exception("package $PACKAGE_NAME not exist delivery directory: $PACKAGE_PATH")
            }

            project(":$ANDROID_APP_MAIN_PROJECT:$PACKAGE_NAME").projectDir = File(PACKAGE_PATH)
        }
    }
}

includeLibrary("MENGINE_APP_LIBRARY_MENGINE", ":libraries:Mengine")
includeLibrary("MENGINE_APP_LIBRARY_OPENAL32", ":libraries:OpenAL32")

/*****************************************************************************
/ * - MENGINE_APP_PLUGIN_FIREBASE [https://firebase.google.com]
/ * - MENGINE_APP_PLUGIN_FIREBASE_APPCHECK [https://firebase.google.com/docs/app-check]
/ * - MENGINE_APP_PLUGIN_FIREBASE_ANALYTICS [https://firebase.google.com/docs/analytics]
/ * - MENGINE_APP_PLUGIN_FIREBASE_CRASHLYTICS [https://firebase.google.com/docs/crashlytics]
/ * - MENGINE_APP_PLUGIN_FIREBASE_MESSAGING [https://firebase.google.com/docs/cloud-messaging]
/ * - MENGINE_APP_PLUGIN_FIREBASE_REMOTECONFIG [https://firebase.google.com/docs/remote-config]
/ * - MENGINE_APP_PLUGIN_FIREBASE_PERFORMANCEMONITORING [https://firebase.google.com/docs/perf-mon]
/ * - MENGINE_APP_PLUGIN_APPMETRICA [https://yandex.ru/dev/appmetrica]
/ * - MENGINE_APP_PLUGIN_APPSFLYER [https://dev.appsflyer.com]
/ * - MENGINE_APP_PLUGIN_FLURRY [https://www.flurry.com]
/ * - MENGINE_APP_PLUGIN_ADMOB [https://admob.google.com]
/ * - MENGINE_APP_PLUGIN_AMAZON [https://developer.amazon.com]
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
/ * - MENGINE_APP_PLUGIN_LEANPLUM [https://www.leanplum.com]
/ * - MENGINE_APP_PLUGIN_DATADOG [https://www.datadoghq.com]
*****************************************************************************/

includePlugin("MENGINE_APP_PLUGIN_SPLASHSCREEN", ":plugins:SplashScreen")
includePlugin("MENGINE_APP_PLUGIN_LOCAL_NOTIFICATIONS", ":plugins:LocalNotifications")
includePlugin("MENGINE_APP_PLUGIN_ADVERTISING", ":plugins:Advertising")
includePlugin("MENGINE_APP_PLUGIN_GDPR", ":plugins:GeneralDataProtectionRegulation")
includePlugin("MENGINE_APP_PLUGIN_GOOGLE_SERVICE", ":plugins:GoogleService")
includePlugin("MENGINE_APP_PLUGIN_GOOGLE_PLAY_BILLING", ":plugins:GooglePlayBilling")
includePlugin("MENGINE_APP_PLUGIN_GOOGLE_GAME_SOCIAL", ":plugins:GoogleGameSocial")
includePlugin("MENGINE_APP_PLUGIN_GOOGLE_INAPP_REVIEWS", ":plugins:GoogleInAppReviews")
includePlugin("MENGINE_APP_PLUGIN_GOOGLE_CONSENT", ":plugins:GoogleConsent")
includePlugin("MENGINE_APP_PLUGIN_FIREBASE", ":plugins:Firebase")
includePlugin("MENGINE_APP_PLUGIN_FIREBASE_APPCHECK", ":plugins:FirebaseAppCheck")
includePlugin("MENGINE_APP_PLUGIN_FIREBASE_ANALYTICS", ":plugins:FirebaseAnalytics")
includePlugin("MENGINE_APP_PLUGIN_FIREBASE_CRASHLYTICS", ":plugins:FirebaseCrashlytics")
includePlugin("MENGINE_APP_PLUGIN_FIREBASE_MESSAGING", ":plugins:FirebaseMessaging")
includePlugin("MENGINE_APP_PLUGIN_FIREBASE_REMOTECONFIG", ":plugins:FirebaseRemoteConfig")
includePlugin("MENGINE_APP_PLUGIN_FIREBASE_PERFORMANCEMONITORING", ":plugins:FirebasePerformanceMonitoring")
includePlugin("MENGINE_APP_PLUGIN_APPMETRICA", ":plugins:AppMetrica")
includePlugin("MENGINE_APP_PLUGIN_APPSFLYER", ":plugins:AppsFlyer")
includePlugin("MENGINE_APP_PLUGIN_FLURRY", ":plugins:Flurry")
includePlugin("MENGINE_APP_PLUGIN_ADMOB", ":plugins:AdMob")
includePlugin("MENGINE_APP_PLUGIN_AMAZON", ":plugins:Amazon")
includePlugin("MENGINE_APP_PLUGIN_APPLOVIN", ":plugins:AppLovin")
includePlugin("MENGINE_APP_PLUGIN_SENTRY", ":plugins:Sentry")
includePlugin("MENGINE_APP_PLUGIN_FACEBOOK", ":plugins:Facebook")
includePlugin("MENGINE_APP_PLUGIN_DEVTODEV", ":plugins:DevToDev")
includePlugin("MENGINE_APP_PLUGIN_MAR", ":plugins:MAR")
includePlugin("MENGINE_APP_PLUGIN_ADJUST", ":plugins:Adjust")
includePlugin("MENGINE_APP_PLUGIN_HELPSHIFT", ":plugins:Helpshift")
includePlugin("MENGINE_APP_PLUGIN_ONESIGNAL", ":plugins:OneSignal")
includePlugin("MENGINE_APP_PLUGIN_LEANPLUM", ":plugins:Leanplum")
includePlugin("MENGINE_APP_PLUGIN_DEVDEBUGGER", ":plugins:DevDebugger")
includePlugin("MENGINE_APP_PLUGIN_DATADOG", ":plugins:DataDog")

println("Mengine complete settings")