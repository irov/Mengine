# Android AdMob

Ad units come from the Remote Config JSON parameter `admob_ad_units`:

```json
{"banner":"","topper":"","interstitial":"","rewarded":"","rewarded_interstitial":""}
```

Use Android ad unit IDs in the Android condition of this parameter. iOS uses the
same parameter name with its own IDs. An absent or empty format disables that
format. A non-string ID invalidates the object for the session.

The plugin reads the activated cache and bundled RC defaults once, after
application services have loaded and transparency consent has resolved. It does
not wait for a network fetch. Later RC activation does not replace ad units or
restart the SDK during the session. If the parameter is missing, initialization
is skipped until the next application launch. `AdServiceReady` is also released
when initialization is skipped, so game startup can continue without ads.
`isSdkInitialized` separately reports whether SDK initialization succeeded.

Ad objects live for one Activity. Once the SDK is ready, the plugin creates them
for the current Activity; `onDestroy` releases them and clears the plugin's
references. A new Activity gets new objects using the same startup RC snapshot.
Destruction detaches pending fullscreen load callbacks and cancels queued shows
and retries. The SDK network request itself has no cancellation API.

The SDK application ID stays in `mengine_google_ads_app_id`. Ad unit resources
and service-config overrides are no longer used. Put temporary test IDs in RC;
there is no test-ID substitution in the plugin.

Enable `MENGINE_APP_PLUGIN_ADMOB_BANNERAD` for banner and topper,
`MENGINE_APP_PLUGIN_ADMOB_INTERSTITIALAD` for interstitial,
`MENGINE_APP_PLUGIN_ADMOB_REWARDEDAD` for rewarded, and
`MENGINE_APP_PLUGIN_ADMOB_REWARDEDINTERSTITIALAD` for rewarded interstitial.

`MengineAdService` exposes separate banner/topper availability, loaded state,
show/hide and pixel dimensions. Each banner owns its SDK view, including
when both use the same ad unit ID. The regular banner stays at the bottom of
the native container; the topper stays at the top. LayoutBox reads the loaded
height and reserves space without positioning the native view. `showBanner`,
`hideBanner`, `showTopper` and `hideTopper` are void commands; Python uses
`androidMethod` for these methods. Check loaded state separately.

Rewarded interstitial has its own SDK object and explicit show methods. It uses
rewarded ad points and the existing rewarded callback stream. Reward delivery
comes from the SDK reward callback; dismissal alone does not grant a reward.
There is no automatic fallback between fullscreen formats. The caller owns the
reward/skip introduction required before displaying a rewarded interstitial.

The shared AppLovin provider also supports fixed bottom/top banners. Its topper ID
comes from `applovin_ad_units.topper`, read once at application startup. Its other
formats retain their existing configuration; rewarded interstitial is unsupported.
