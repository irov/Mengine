package org.Mengine.Base;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.lifecycle.DefaultLifecycleObserver;
import androidx.lifecycle.LifecycleOwner;
import androidx.lifecycle.ProcessLifecycleOwner;

import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.Set;

public class MengineAdService extends MengineService implements DefaultLifecycleObserver, MengineAdProviderInterface, MengineAdResponseInterface, MengineListenerActivity, MengineListenerRemoteConfig, MengineListenerInAppPurchase {
    public static final String SERVICE_NAME = "AdService";
    public static final boolean SERVICE_EMBEDDING = true;
    public static final int SAVE_VERSION = 1;

    public static final String INAPPURCHASE_DISABLE_ADS = "disable_ads";

    private final Map<String, MengineAdPointInterstitial> m_adInterstitialPoints = new HashMap<>();
    private final Map<String, MengineAdPointRewarded> m_adRewardedPoints = new HashMap<>();
    private final Map<String, MengineAdPointAppOpen> m_adAppOpenPoints = new HashMap<>();

    private final Map<String, MengineAdCooldown> m_adCooldowns = new HashMap<>();
    private final Map<String, MengineAdAttempts> m_adAttempts = new HashMap<>();

    private final Object m_syncronizationAdPoints = new Object();

    private MengineAdProviderInterface m_adProvider;

    protected long m_timestampStop = -1;

    protected long m_lastShowInterstitial = -1;
    protected long m_lastShowRewarded = -1;
    protected long m_lastShowAppOpen = -1;

    protected long m_countShowInterstitial = 0;
    protected long m_countShowRewarded = 0;
    protected long m_countShowAppOpen = 0;

    protected boolean m_optionNoAds = false;
    protected boolean m_noAds = false;

    public void setAdProvider(MengineAdProviderInterface adProvider) {
        m_adProvider = adProvider;
    }

    public MengineAdProviderInterface getAdProvider() {
        return m_adProvider;
    }

    public MengineAdResponseInterface getAdResponse() {
        return this;
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        m_optionNoAds = this.hasOption("ad.no_ads");

        m_noAds = MengineFragmentInAppPurchase.INSTANCE.isOwnedInAppProduct(INAPPURCHASE_DISABLE_ADS);

        if (this.hasAppOpen() == true) {
            ProcessLifecycleOwner.get().getLifecycle().addObserver(this);
        }
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        if (this.hasAppOpen() == true) {
            ProcessLifecycleOwner.get().getLifecycle().removeObserver(this);
        }
    }

    public void readyAdProvider() {
        this.activateSemaphore("AdServiceReady");
    }

    private void setupAdBasePointAttemts(MengineAdPointBase adPoint) {
        String adPointName = adPoint.getName();

        if (m_adAttempts.containsKey(adPointName) == false) {
            MengineAdAttempts newAttempts = new MengineAdAttempts();

            m_adAttempts.put(adPointName, newAttempts);
        }

        MengineAdAttempts attempts = m_adAttempts.get(adPointName);

        adPoint.setAttempts(attempts);
    }

    private void setupAdBasePointCooldown(MengineAdPointBase adPoint) {
        String cooldownGroupName = adPoint.getCooldownGroupName();

        if (cooldownGroupName == null) {
            MengineAdCooldown newCooldown = new MengineAdCooldown();

            adPoint.setCooldown(newCooldown);
        } else {
            if (m_adCooldowns.containsKey(cooldownGroupName) == false) {
                MengineAdCooldown newCooldown = new MengineAdCooldown();

                m_adCooldowns.put(cooldownGroupName, newCooldown);
            }

            MengineAdCooldown cooldown = m_adCooldowns.get(cooldownGroupName);

            adPoint.setCooldown(cooldown);
        }
    }

    private MengineAdPointInterstitial getAdInterstitialPoint(String adPointName) {
        synchronized (m_syncronizationAdPoints) {
            return m_adInterstitialPoints.getOrDefault(adPointName, null);
        }
    }

    private MengineAdPointRewarded getAdRewardedPoint(String adPointName) {
        synchronized (m_syncronizationAdPoints) {
            return m_adRewardedPoints.getOrDefault(adPointName, null);
        }
    }

    private MengineAdPointAppOpen getAdAppOpenPoint(String adPointName) {
        synchronized (m_syncronizationAdPoints) {
            return m_adAppOpenPoints.getOrDefault(adPointName, null);
        }
    }

    private void parseAdInterstitialPoint(String adPointName, JSONObject adPointConfig) {
        if (m_adInterstitialPoints.containsKey(adPointName) == true) {
            this.logError("ad interstitial point '%s' already exists", adPointName);

            return;
        }

        MengineAdPointInterstitial adPoint = new MengineAdPointInterstitial(adPointName, adPointConfig);

        this.setupAdBasePointCooldown(adPoint);
        this.setupAdBasePointAttemts(adPoint);

        m_adInterstitialPoints.put(adPointName, adPoint);
    }

    private void parseAdRewardedPoint(String adPointName, JSONObject adPointConfig) {
        if (m_adRewardedPoints.containsKey(adPointName) == true) {
            this.logError("ad rewarded point '%s' already exists", adPointName);

            return;
        }

        MengineAdPointRewarded adPoint = new MengineAdPointRewarded(adPointName, adPointConfig);

        this.setupAdBasePointCooldown(adPoint);
        this.setupAdBasePointAttemts(adPoint);

        m_adRewardedPoints.put(adPointName, adPoint);
    }

    private void parseAdAppOpenPoint(String adPointName, JSONObject adPointConfig) {
        if (m_adAppOpenPoints.containsKey(adPointName) == true) {
            this.logError("ad appopen point '%s' already exists", adPointName);

            return;
        }

        MengineAdPointAppOpen adPoint = new MengineAdPointAppOpen(adPointName, adPointConfig);

        this.setupAdBasePointCooldown(adPoint);
        this.setupAdBasePointAttemts(adPoint);

        m_adAppOpenPoints.put(adPointName, adPoint);
    }

    @Override
    public void onMengineRemoteConfigFetch(@NonNull MengineApplication application, boolean updated, @NonNull Map<String, JSONObject> remoteConfig) {
        synchronized (m_syncronizationAdPoints) {
            m_adInterstitialPoints.clear();
            m_adRewardedPoints.clear();
            m_adAppOpenPoints.clear();
            m_adCooldowns.clear();

            String ad_interstitial_prefix = "ad_interstitial_";
            String ad_rewarded_prefix = "ad_rewarded_";
            String ad_appopen_prefix = "ad_appopen_";

            for (Map.Entry<String, JSONObject> entry : remoteConfig.entrySet()) {
                String key = entry.getKey();
                JSONObject adPointConfig = entry.getValue();

                if (key.startsWith(ad_interstitial_prefix) == true) {
                    String adPointName = key.substring(ad_interstitial_prefix.length());

                    this.parseAdInterstitialPoint(adPointName, adPointConfig);
                } else if (key.startsWith(ad_rewarded_prefix) == true) {
                    String adPointName = key.substring(ad_rewarded_prefix.length());

                    this.parseAdRewardedPoint(adPointName, adPointConfig);
                } else if (key.startsWith(ad_appopen_prefix) == true) {
                    String adPointName = key.substring(ad_appopen_prefix.length());

                    this.parseAdAppOpenPoint(adPointName, adPointConfig);
                }
            }
        }
    }

    @Override
    public Bundle onSave(@NonNull MengineApplication application) {
        Bundle bundle = new Bundle();

        bundle.putInt("version", SAVE_VERSION);

        Bundle attemptsBundle = new Bundle();

        for (Map.Entry<String, MengineAdAttempts> entry : m_adAttempts.entrySet()) {
            String attemptsPointName = entry.getKey();
            MengineAdAttempts attempts = entry.getValue();

            Bundle attemptsPointBundle = attempts.onSave(application);

            attemptsBundle.putBundle(attemptsPointName, attemptsPointBundle);
        }

        bundle.putBundle("attempts", attemptsBundle);

        Bundle cooldownsBundle = new Bundle();

        for (Map.Entry<String, MengineAdCooldown> entry : m_adCooldowns.entrySet()) {
            String cooldownGroupName = entry.getKey();
            MengineAdCooldown cooldown = entry.getValue();

            Bundle cooldownBundle = cooldown.onSave(application);

            cooldownsBundle.putBundle(cooldownGroupName, cooldownBundle);
        }

        bundle.putBundle("cooldowns", cooldownsBundle);

        return bundle;
    }

    @Override
    public void onLoad(@NonNull MengineApplication application, @NonNull Bundle bundle) {
        int version = bundle.getInt("version", 0);

        Bundle attemptsBundle = bundle.getBundle("attempts");

        if (attemptsBundle != null) {
            for (String key : attemptsBundle.keySet()) {
                Bundle attemptsPointBundle = attemptsBundle.getBundle(key);

                MengineAdAttempts attempts = new MengineAdAttempts();

                attempts.onLoad(application, attemptsPointBundle);

                m_adAttempts.put(key, attempts);
            }
        }

        Bundle cooldownsBundle = bundle.getBundle("cooldowns");

        if (cooldownsBundle != null) {
            for (String key : cooldownsBundle.keySet()) {
                Bundle cooldownBundle = cooldownsBundle.getBundle(key);

                MengineAdCooldown cooldown = new MengineAdCooldown();

                cooldown.onLoad(application, cooldownBundle);

                m_adCooldowns.put(key, cooldown);
            }
        }
    }

    @Override
    public void onStart(@NonNull LifecycleOwner owner) {
        if (m_timestampStop == -1) {
            return;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return;
        }

        String placement = "start";

        long timeStop = MengineUtils.getTimestamp() - m_timestampStop;

        if (this.canYouShowAppOpen(placement, timeStop) == false) {
            return;
        }

        if (this.showAppOpen(placement) == false) {
            return;
        }
    }

    @Override
    public void onStop(@NonNull LifecycleOwner owner) {
        m_timestampStop = MengineUtils.getTimestamp();
    }

    public void setNoAds(boolean noAds) {
        if (m_noAds == noAds) {
            return;
        }

        m_noAds = noAds;

        if (m_adProvider != null) {
            m_adProvider.hideBanner();
        }
    }

    public boolean getNoAds() {
        if (m_optionNoAds == true) {
            return true;
        }

        if (m_noAds == true) {
            return true;
        }

        return false;
    }

    @Override
    public boolean hasBanner() {
        if (m_adProvider == null) {
            return false;
        }

        return m_adProvider.hasBanner();
    }

    @Override
    public boolean canYouShowBanner() {
        if (m_adProvider == null) {
            return false;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return false;
        }

        if (m_adProvider.canYouShowBanner() == false) {
            return false;
        }

        return true;
    }

    @Override
    public void showBanner() {
        if (m_adProvider == null) {
            return;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return;
        }

        m_adProvider.showBanner();
    }

    @Override
    public void hideBanner() {
        if (m_adProvider == null) {
            return;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return;
        }

        m_adProvider.hideBanner();
    }

    @Override
    public int getBannerWidth() {
        if (m_adProvider == null) {
            return 0;
        }

        int width = m_adProvider.getBannerWidth();

        return width;
    }

    @Override
    public int getBannerHeight() {
        if (m_adProvider == null) {
            return 0;
        }

        int height = m_adProvider.getBannerHeight();

        return height;
    }

    @Override
    public boolean hasInterstitial() {
        if (m_adProvider == null) {
            return false;
        }

        if (m_adProvider.hasInterstitial() == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canYouShowInterstitial(String placement) {
        MengineAdPointInterstitial adPoint = this.getAdInterstitialPoint(placement);

        if (adPoint == null) {
            this.logError("ad interstitial point '%s' not found", placement);

            return false;
        }

        if (m_adProvider == null) {
            return false;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return false;
        }

        MengineApplication application = MengineApplication.INSTANCE;

        if (adPoint.canYouShowAd(application) == false) {
            return false;
        }

        if (m_adProvider.canYouShowInterstitial(placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean showInterstitial(String placement) {
        MengineAdPointInterstitial adPoint = this.getAdInterstitialPoint(placement);

        if (adPoint == null) {
            this.logError("ad interstitial point '%s' not found", placement);

            return false;
        }

        if (m_adProvider == null) {
            return false;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return false;
        }

        if (m_adProvider.showInterstitial(placement) == false) {
            return false;
        }

        m_lastShowInterstitial = MengineUtils.getTimestamp();
        m_countShowInterstitial += 1;

        adPoint.showAd();

        return true;
    }

    @Override
    public boolean hasRewarded() {
        if (m_adProvider == null) {
            return false;
        }

        if (m_adProvider.hasRewarded() == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canOfferRewarded(String placement) {
        MengineAdPointRewarded adPoint = this.getAdRewardedPoint(placement);

        if (adPoint == null) {
            this.logError("ad rewarded point '%s' not found", placement);

            return false;
        }

        if (m_adProvider == null) {
            return false;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return false;
        }

        MengineApplication application = MengineApplication.INSTANCE;

        if (adPoint.canOfferAd(application) == false) {
            return false;
        }

        if (m_adProvider.canOfferRewarded(placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canYouShowRewarded(String placement) {
        MengineAdPointRewarded adPoint = this.getAdRewardedPoint(placement);

        if (adPoint == null) {
            this.logError("ad rewarded point '%s' not found", placement);

            return false;
        }

        if (m_adProvider == null) {
            return false;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return false;
        }

        MengineApplication application = MengineApplication.INSTANCE;

        if (adPoint.canYouShowAd(application) == false) {
            return false;
        }

        if (m_adProvider.canYouShowRewarded(placement) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean showRewarded(String placement) {
        MengineAdPointRewarded adPoint = this.getAdRewardedPoint(placement);

        if (adPoint == null) {
            this.logError("ad rewarded point '%s' not found", placement);

            return false;
        }

        if (m_adProvider == null) {
            return false;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return false;
        }

        if (m_adProvider.showRewarded(placement) == false) {
            return false;
        }

        m_lastShowRewarded = MengineUtils.getTimestamp();
        m_countShowRewarded += 1;

        adPoint.showAd();

        return true;
    }

    @Override
    public boolean hasAppOpen() {
        if (m_adProvider == null) {
            return false;
        }

        return m_adProvider.hasAppOpen();
    }

    @Override
    public boolean canYouShowAppOpen(String placement, long timeStop) {
        MengineAdPointAppOpen adPoint = this.getAdAppOpenPoint(placement);

        if (adPoint == null) {
            this.logError("ad appopen point '%s' not found", placement);

            return false;
        }

        if (m_adProvider == null) {
            return false;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return false;
        }

        MengineApplication application = MengineApplication.INSTANCE;

        if (adPoint.canYouShowAd(application, timeStop) == false) {
            return false;
        }

        if (m_adProvider.canYouShowAppOpen(placement, timeStop) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean showAppOpen(String placement) {
        MengineAdPointAppOpen adPoint = this.getAdAppOpenPoint(placement);

        if (adPoint == null) {
            this.logError("ad appopen point '%s' not found", placement);

            return false;
        }

        if (m_adProvider == null) {
            return false;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return false;
        }

        if (m_adProvider.showAppOpen(placement) == false) {
            return false;
        }

        m_lastShowAppOpen = MengineUtils.getTimestamp();
        m_countShowAppOpen += 1;

        adPoint.showAd();

        return true;
    }

    @Override
    public boolean hasMREC() {
        if (m_adProvider == null) {
            return false;
        }

        return m_adProvider.hasMREC();
    }

    @Override
    public boolean canYouShowMREC() {
        if (m_adProvider == null) {
            return false;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return false;
        }

        if (m_adProvider.canYouShowMREC() == false) {
            return false;
        }

        return true;
    }

    @Override
    public void showMREC(int leftMargin, int topMargin) {
        if (m_adProvider == null) {
            return;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return;
        }

        m_adProvider.showMREC(leftMargin, topMargin);
    }

    @Override
    public void hideMREC() {
        if (m_adProvider == null) {
            return;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return;
        }

        m_adProvider.hideMREC();
    }

    @Override
    public int getMRECLeftMargin() {
        if (m_adProvider == null) {
            return 0;
        }

        int leftMargin = m_adProvider.getMRECLeftMargin();

        return leftMargin;
    }

    @Override
    public int getMRECTopMargin() {
        if (m_adProvider == null) {
            return 0;
        }

        int topMargin = m_adProvider.getMRECTopMargin();

        return topMargin;
    }

    @Override
    public int getMRECWidth() {
        if (m_adProvider == null) {
            return 0;
        }

        int width = m_adProvider.getMRECWidth();

        return width;
    }

    @Override
    public int getMRECHeight() {
        if (m_adProvider == null) {
            return 0;
        }

        int height = m_adProvider.getMRECHeight();

        return height;
    }

    @Override
    public boolean hasNative() {
        if (m_adProvider == null) {
            return false;
        }

        return m_adProvider.hasNative();
    }

    @Override
    public boolean canYouShowNative() {
        if (m_adProvider == null) {
            return false;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return false;
        }

        if (m_adProvider.canYouShowNative() == false) {
            return false;
        }

        return true;
    }

    @Override
    public void showNative() {
        if (m_adProvider == null) {
            return;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return;
        }

        m_adProvider.showNative();
    }

    @Override
    public void hideNative() {
        if (m_adProvider == null) {
            return;
        }

        boolean noAds = this.getNoAds();

        if (noAds == true) {
            return;
        }

        m_adProvider.hideNative();
    }

    @Override
    public int getNativeLeftMargin() {
        if (m_adProvider == null) {
            return 0;
        }

        int leftMargin = m_adProvider.getNativeLeftMargin();

        return leftMargin;
    }

    @Override
    public int getNativeTopMargin() {
        if (m_adProvider == null) {
            return 0;
        }

        int topMargin = m_adProvider.getNativeTopMargin();

        return topMargin;
    }

    @Override
    public int getNativeWidth() {
        if (m_adProvider == null) {
            return 0;
        }

        int width = m_adProvider.getNativeWidth();

        return width;
    }

    @Override
    public int getNativeHeight() {
        if (m_adProvider == null) {
            return 0;
        }

        int height = m_adProvider.getNativeHeight();

        return height;
    }

    @Override
    public void onAdRevenuePaid(@NonNull MengineAdMediation mediation, @NonNull MengineAdFormat format, String placement, double revenue) {
        Map<String, Object> params = Map.of("placement", placement, "revenue", revenue);

        this.increaseStatisticDouble("ad.total.revenue", revenue);
        this.increaseStatisticDouble("ad.total.revenue." + placement, revenue);

        if (format == MengineAdFormat.ADFORMAT_BANNER) {
            this.increaseStatisticDouble("ad.banner.revenue", revenue);
            this.increaseStatisticDouble("ad.banner.revenue." + placement, revenue);

            this.nativeCall("onAndroidAdServiceBannerRevenuePaid", params);
        } else if (format == MengineAdFormat.ADFORMAT_INTERSTITIAL) {
            this.increaseStatisticDouble("ad.interstitial.revenue", revenue);
            this.increaseStatisticDouble("ad.interstitial.revenue." + placement, revenue);

            this.nativeCall("onAndroidAdServiceInterstitialRevenuePaid", params);
        } else if (format == MengineAdFormat.ADFORMAT_REWARDED) {
            this.increaseStatisticDouble("ad.rewarded.revenue", revenue);
            this.increaseStatisticDouble("ad.rewarded.revenue." + placement, revenue);

            this.nativeCall("onAndroidAdServiceRewardedRevenuePaid", params);
        } else if (format == MengineAdFormat.ADFORMAT_APPOPEN) {
            this.increaseStatisticDouble("ad.appopen.revenue", revenue);
            this.increaseStatisticDouble("ad.appopen.revenue." + placement, revenue);

            this.nativeCall("onAndroidAdServiceAppOpenRevenuePaid", params);
        } else if (format == MengineAdFormat.ADFORMAT_MREC) {
            this.increaseStatisticDouble("ad.mrec.revenue", revenue);
            this.increaseStatisticDouble("ad.mrec.revenue." + placement, revenue);

            this.nativeCall("onAndroidAdServiceMRECRevenuePaid", params);
        }
    }

    @Override
    public void onAdShowSuccess(@NonNull MengineAdMediation mediation, @NonNull MengineAdFormat format, String placement) {
        Map<String, Object> params = Map.of("placement", placement);

        if (format == MengineAdFormat.ADFORMAT_INTERSTITIAL) {
            this.increaseStatisticInteger("ad.interstitial.show", 1);
            this.increaseStatisticInteger("ad.interstitial.show." + placement, 1);

            this.nativeCall("onAndroidAdServiceInterstitialShowSuccess", params);
        } else if (format == MengineAdFormat.ADFORMAT_REWARDED) {
            this.increaseStatisticInteger("ad.rewarded.show", 1);
            this.increaseStatisticInteger("ad.rewarded.show." + placement, 1);

            this.nativeCall("onAndroidAdServiceRewardedShowSuccess", params);
        } else if (format == MengineAdFormat.ADFORMAT_APPOPEN) {
            this.increaseStatisticInteger("ad.appopen.show", 1);
            this.increaseStatisticInteger("ad.appopen.show." + placement, 1);

            this.nativeCall("onAndroidAdServiceAppOpenShowSuccess", params);
        } else if (format == MengineAdFormat.ADFORMAT_MREC) {
            this.increaseStatisticInteger("ad.mrec.show", 1);
            this.increaseStatisticInteger("ad.mrec.show." + placement, 1);

            this.nativeCall("onAndroidAdServiceMRECShowSuccess", params);
        }
    }

    @Override
    public void onAdShowFailed(@NonNull MengineAdMediation mediation, @NonNull MengineAdFormat format, String placement, int errorCode) {
        Map<String, Object> params = Map.of("placement", placement, "error_code", errorCode);

        if (format == MengineAdFormat.ADFORMAT_INTERSTITIAL) {
            this.nativeCall("onAndroidAdServiceInterstitialShowFailed", params);
        } else if (format == MengineAdFormat.ADFORMAT_REWARDED) {
            this.nativeCall("onAndroidAdServiceRewardedShowFailed", params);
        } else if (format == MengineAdFormat.ADFORMAT_APPOPEN) {
            this.nativeCall("onAndroidAdServiceAppOpenShowFailed", params);
        } else if (format == MengineAdFormat.ADFORMAT_MREC) {
            this.nativeCall("onAndroidAdServiceMRECShowFailed", params);
        }
    }

    @Override
    public void onAdUserRewarded(@NonNull MengineAdMediation mediation, @NonNull MengineAdFormat format, String placement, String label, int amount) {
        Map<String, Object> params = Map.of("placement", placement, "label", label, "amount", amount);

        if (format == MengineAdFormat.ADFORMAT_REWARDED) {
            this.increaseStatisticInteger("ad.rewarded.user_rewarded", 1);
            this.increaseStatisticInteger("ad.rewarded.user_rewarded." + placement, 1);

            this.nativeCall("onAndroidAdServiceRewardedUserRewarded", params);
        }
    }

    @Override
    public void onMenginePurchaseInAppProduct(@NonNull MengineApplication application, @NonNull MengineParamInAppPurchase purchase) {
        for (String product : purchase.INAPPPURCHASE_PRODUCTS) {
            if (Objects.equals(product, INAPPURCHASE_DISABLE_ADS) == false) {
                continue;
            }

            this.logInfo("purchase in app product '%s' for disable ads", product);

            this.setNoAds(true);
        }
    }

    public void onMengineOwnedInAppProducts(@NonNull MengineApplication application, @NonNull Set<String> products) {
        boolean noAds = products.contains(INAPPURCHASE_DISABLE_ADS);

        this.setNoAds(noAds);
    }
}