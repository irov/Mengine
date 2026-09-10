# AppLovin ad units

Ad unit IDs come only from the cached/default Remote Config object
`applovin_ad_units`, read once during provider initialization. IDs are passed to
ad constructors. There is no fallback to Android `*_adunitid` string resources,
service-config `*AdUnitId` keys or iOS bundle `*AdUnitId` keys.

```json
{
  "banner": "<bottom-banner-id>",
  "topper": "<top-banner-id>",
  "interstitial": "<interstitial-id>",
  "rewarded": "<rewarded-id>",
  "appopen": "<app-open-id>",
  "mrec": "<mrec-id>",
  "native": "<native-id>"
}
```

Android supports all seven entries. The iOS plugin currently supports `banner`,
`topper`, `interstitial` and `rewarded`. Omit unused entries. Missing or empty IDs
create no ad object for that format. A non-string ID is logged and rejects the ID
map for the session. No format borrows another format's ID. SDK/CMP initialization
still runs when the map is absent so consent can finish.

Each Android format has its existing `MENGINE_APP_PLUGIN_APPLOVIN_*AD` build flag.
`MENGINE_APP_PLUGIN_APPLOVIN_BANNERAD` and `MENGINE_APP_PLUGIN_APPLOVIN_TOPPERAD`
are independent: either, both, or neither can be enabled. They use the shared
BannerAd module with separate objects, placements and IDs. iOS has corresponding
`MENGINE_PLUGIN_IOS_APPLOVIN_BANNER` and `MENGINE_PLUGIN_IOS_APPLOVIN_TOPPER` options.

No-ads entitlement suppresses the non-rewarded formats as before. SDK key,
placement/layout and privacy settings keep their existing configuration paths.

On iOS initialization reads RC on the main queue after plugin post-launch callbacks
have populated the cached/default values. No live RC subscription is introduced;
changing IDs takes effect on the next application launch.
