-keep class com.devtodev.** { *; }
-dontwarn com.devtodev.**

# Required by devtodev's Google Mobile Services integration.
# https://docs.devtodev.com/integration/integration-of-sdk-v2/sdk-integration/android
-keep class com.google.android.gms.** { *; }
