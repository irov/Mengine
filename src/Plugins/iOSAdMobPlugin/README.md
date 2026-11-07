# AdMob ad units on iOS

Configure ad unit IDs in the Remote Config JSON parameter `admob_ad_units`:

```json
{"banner":"YOUR_BANNER_AD_UNIT_ID","topper":"YOUR_TOPPER_AD_UNIT_ID","interstitial":"YOUR_INTERSTITIAL_AD_UNIT_ID","rewarded":"YOUR_REWARDED_AD_UNIT_ID","rewarded_interstitial":"YOUR_REWARDED_INTERSTITIAL_AD_UNIT_ID"}
```

Publish this parameter in Firebase Remote Config. Ad unit IDs are managed only
there; do not copy them into the application's bundled defaults or `Info.plist`.
The iOS Firebase Remote Config plugin discovers activated server keys without
requiring them to be registered in the defaults plist.

Move the IDs from `MengineiOSAdMobPlugin.BannerAdUnitId`, `InterstitialAdUnitId`,
`RewardedAdUnitId`, and `RewardedInterstitialAdUnitId` in `Info.plist` to the
corresponding JSON fields. Those `Info.plist` keys and `TestAdUnitsEnabled` are
no longer read. Keep `GADApplicationIdentifier` and the remaining plugin settings
in `Info.plist`.

After consent allows requests and the ATT flow completes, AdMob reads
`admob_ad_units` once through `iOSFirebaseRemoteConfigInterface`. It uses the
currently activated values already available in the Remote Config cache and does
not wait for a fetch or subscribe to `onConfig:ids:`. The selected IDs remain fixed
for the session, including while asynchronous SDK initialization completes.
Remote Config updates after this read can affect ads on the next application
launch. Other consumers continue to receive Remote Config updates normally.

Missing fields and empty strings disable the corresponding formats. An empty
object disables all formats. The existing CMake options must also enable each
format. A missing object, a non-object value, or a non-string ID skips AdMob
initialization for the session. This also applies to a fresh install with no
activated configuration yet; fetching it later does not start ads in that session.

To use test ads temporarily, put the appropriate [Google demo ad unit IDs](https://developers.google.com/admob/ios/test-ads#demo_ad_units)
in this same Remote Config parameter. Test and production IDs follow the same
request path.

There are two fixed banners: `banner` and `topper`. Each owns one SDK view and uses
its own ad unit ID. Both use `MENGINE_PLUGIN_IOS_ADMOB_BANNER`; an absent or empty
`topper` disables only the topper. Banner placement dictionaries and
`ad_banner_*` enable parameters are no longer used.

Python uses `iOSAdvertisementShowBanner`, `iOSAdvertisementHideBanner`,
`iOSAdvertisementIsBannerLoaded`, and `iOSAdvertisementGetBannerHeight` for
the regular banner. The topper exposes the corresponding `Topper` methods.
The regular banner stays at the bottom of the safe area; the topper stays at
the top. Python controls their visibility. LayoutBox only reads their loaded
heights in pixels and reserves space; it never positions the native ad views.

The optional AppLovin provider reads its topper ID once from
`applovin_ad_units.topper`; its existing configuration for other formats is unchanged.

## Test devices

In a Debug build, keep `RequestConfigurationTestDeviceIdsEnabled` enabled and
use the existing `admob.test_device_advertising` option to register the current
device automatically. No manual device-ID list is required.

The plugin matches Google Mobile Ads 13.11.0: it uses the MD5 of the IDFA UUID
string, falling back to the IDFV UUID string when IDFA is unavailable after ATT.
The SDK identifier uses lowercase hexadecimal. The plugin uses the portable
`Mengine::Helper::makeMD5String` helper in `Kernel`, backed by
[`irov/md5`](https://github.com/irov/md5). IDFA/IDFV selection and test-device
registration stay in the Objective-C++ plugin. Compare the plugin's test ID
with the ID printed by the SDK when validating a new SDK version on a device.

If neither identifier is available, the plugin logs the condition and continues
normal SDK initialization. Release builds ignore the programmatic test setup.
Devices may also be registered through the AdMob console, which requests IDFA.
See the [Google testing guide](https://developers.google.com/admob/ios/test-ads).
