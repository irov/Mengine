# APS does not bundle consumer rules; keep its reflective integration points.
# https://github.com/AppLovin/AppLovin-MAX-SDK-Android/blob/master/AmazonAdMarketplace/proguard-rules.pro
-keep class com.amazon.device.ads.** { *; }
-keep class com.iabtcf.** { *; }
-keep class com.amazon.aps.** { *; }

# APS 12.x checks for the optional Google Mobile Ads Next Gen SDK at runtime.
-dontwarn com.google.android.libraries.ads.mobile.sdk.**

# OM SDK checks for this optional Fire TV system library before attestation.
# https://compliance.iabtechnologylab.com/integration-guide/android/device-attestation.html
-dontwarn com.amazon.privacypass.**
