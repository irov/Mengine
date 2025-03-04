package org.Mengine.Base;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.lifecycle.DefaultLifecycleObserver;
import androidx.lifecycle.LifecycleOwner;
import androidx.lifecycle.ProcessLifecycleOwner;

import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

public class MengineAdService extends MengineService implements DefaultLifecycleObserver, MengineAdProviderInterface, MengineListenerActivity, MengineListenerRemoteConfig {
    public static final String SERVICE_NAME = "AdService";
    public static final boolean SERVICE_EMBEDDING = true;
    public static final int SAVE_VERSION = 1;

    private final Map<String, MengineAdInterstitialPoint> m_adInterstitialPoints = new HashMap<>();
    private final Map<String, MengineAdRewardedPoint> m_adRewardedPoints = new HashMap<>();
    private final Map<String, MengineAdAppOpenPoint> m_adAppOpenPoints = new HashMap<>();
    private final Map<String, MengineAdCooldown> m_adCooldowns = new HashMap<>();
    private final Map<String, MengineAdAttempts> m_adAttempts = new HashMap<>();

    private MengineAdProviderInterface m_adProvider;

    protected long m_timestampStop = -1;

    protected long m_lastShowInterstitial = -1;
    protected long m_lastShowRewarded = -1;
    protected long m_lastShowAppOpen = -1;

    protected long m_countShowInterstitial = 0;
    protected long m_countShowRewarded = 0;
    protected long m_countShowAppOpen = 0;

    public void setAdProvider(MengineAdProviderInterface adProvider) {
        m_adProvider = adProvider;
    }

    public MengineAdProviderInterface getAdProvider() {
        return m_adProvider;
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
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

    private void setupAdBasePointAttemts(MengineAdBasePoint adPoint) {
        String adPointName = adPoint.getName();

        if (m_adAttempts.containsKey(adPointName) == false) {
            MengineAdAttempts newAttempts = new MengineAdAttempts();

            m_adAttempts.put(adPointName, newAttempts);
        }

        MengineAdAttempts attempts = m_adAttempts.get(adPointName);

        adPoint.setAttempts(attempts);
    }

    private void setupAdBasePointCooldown(MengineAdBasePoint adPoint) {
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

    private void parseInterstitialPoint(String adPointName, JSONObject adPointConfig) {
        if (m_adInterstitialPoints.containsKey(adPointName) == true) {
            this.logError("ad interstitial point '%s' already exists", adPointName);

            return;
        }

        MengineAdInterstitialPoint adPoint = new MengineAdInterstitialPoint(adPointName, adPointConfig);

        this.setupAdBasePointCooldown(adPoint);
        this.setupAdBasePointAttemts(adPoint);

        m_adInterstitialPoints.put(adPointName, adPoint);
    }

    private void parseRewardedPoint(String adPointName, JSONObject adPointConfig) {
        if (m_adRewardedPoints.containsKey(adPointName) == true) {
            this.logError("ad rewarded point '%s' already exists", adPointName);

            return;
        }

        MengineAdRewardedPoint adPoint = new MengineAdRewardedPoint(adPointName, adPointConfig);

        this.setupAdBasePointCooldown(adPoint);
        this.setupAdBasePointAttemts(adPoint);

        m_adRewardedPoints.put(adPointName, adPoint);
    }

    private void parseAppOpenPoint(String adPointName, JSONObject adPointConfig) {
        if (m_adAppOpenPoints.containsKey(adPointName) == true) {
            this.logError("ad appopen point '%s' already exists", adPointName);

            return;
        }

        MengineAdAppOpenPoint adPoint = new MengineAdAppOpenPoint(adPointName, adPointConfig);

        this.setupAdBasePointCooldown(adPoint);
        this.setupAdBasePointAttemts(adPoint);

        m_adAppOpenPoints.put(adPointName, adPoint);
    }

    @Override
    public void onMengineRemoteConfigFetch(@NonNull MengineApplication application, @NonNull Map<String, JSONObject> remoteConfig) {
        synchronized (this) {
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

                    this.parseInterstitialPoint(adPointName, adPointConfig);
                } else if (key.startsWith(ad_rewarded_prefix) == true) {
                    String adPointName = key.substring(ad_rewarded_prefix.length());

                    this.parseRewardedPoint(adPointName, adPointConfig);
                } else if (key.startsWith(ad_appopen_prefix) == true) {
                    String adPointName = key.substring(ad_appopen_prefix.length());

                    this.parseAppOpenPoint(adPointName, adPointConfig);
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

    @Override
    public boolean hasBanner() {
        if (m_adProvider == null) {
            return false;
        }

        return m_adProvider.hasBanner();
    }

    @Override
    public void showBanner() {
        if (m_adProvider == null) {
            return;
        }

        m_adProvider.showBanner();
    }

    @Override
    public void hideBanner() {
        if (m_adProvider == null) {
            return;
        }

        m_adProvider.hideBanner();
    }

    @Override
    public int getBannerHeight() {
        if (m_adProvider == null) {
            return 0;
        }

        int height = m_adProvider.getBannerHeight();

        return height;
    }

    public void bannerRevenuePaid(Map<String, Object> params) {
        double revenue = MengineUtils.getMapDouble(params, "revenue");
        this.increaseStatisticDouble("ad.banner.revenue", revenue);
        this.increaseStatisticDouble("ad.total.revenue", revenue);

        this.pythonCall("onAndroidAdServiceBannerRevenuePaid", params);
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
        if (m_adInterstitialPoints.containsKey(placement) == false) {
            this.logError("ad interstitial point '%s' not found", placement);

            return false;
        }

        if (m_adProvider == null) {
            return false;
        }

        MengineApplication application = this.getMengineApplication();

        MengineAdInterstitialPoint adPoint = m_adInterstitialPoints.get(placement);

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
        if (m_adInterstitialPoints.containsKey(placement) == false) {
            this.logError("ad interstitial point '%s' not found", placement);

            return false;
        }

        if (m_adProvider == null) {
            return false;
        }

        if (m_adProvider.showInterstitial(placement) == false) {
            return false;
        }

        MengineAdInterstitialPoint adPoint = m_adInterstitialPoints.get(placement);

        m_lastShowInterstitial = MengineUtils.getTimestamp();
        m_countShowInterstitial += 1;

        adPoint.showAd();

        return true;
    }

    public void interstitialShowSuccessful(Map<String, Object> params) {
        this.increaseStatisticInteger("ad.interstitial.show", 1);

        this.pythonCall("onAndroidAdServiceInterstitialShowSuccessful", params);
    }

    public void interstitialShowFailed(Map<String, Object> params) {
        this.pythonCall("onAndroidAdServiceInterstitialShowFailed", params);
    }

    public void interstitialRevenuePaid(Map<String, Object> params) {
        double revenue = MengineUtils.getMapDouble(params, "revenue");
        this.increaseStatisticDouble("ad.interstitial.revenue", revenue);
        this.increaseStatisticDouble("ad.total.revenue", revenue);

        this.pythonCall("onAndroidAdServiceInterstitialRevenuePaid", params);
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
        if (m_adRewardedPoints.containsKey(placement) == false) {
            this.logError("ad rewarded point '%s' not found", placement);

            return false;
        }

        if (m_adProvider == null) {
            return false;
        }

        MengineApplication application = this.getMengineApplication();

        MengineAdRewardedPoint adPoint = m_adRewardedPoints.get(placement);

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
        if (m_adRewardedPoints.containsKey(placement) == false) {
            this.logError("ad rewarded point '%s' not found", placement);

            return false;
        }

        if (m_adProvider == null) {
            return false;
        }

        MengineApplication application = this.getMengineApplication();

        MengineAdRewardedPoint adPoint = m_adRewardedPoints.get(placement);

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
        if (m_adRewardedPoints.containsKey(placement) == false) {
            this.logError("ad rewarded point '%s' not found", placement);

            return false;
        }

        if (m_adProvider == null) {
            return false;
        }

        if (m_adProvider.showRewarded(placement) == false) {
            return false;
        }

        m_lastShowRewarded = MengineUtils.getTimestamp();
        m_countShowRewarded += 1;

        MengineAdRewardedPoint adPoint = m_adRewardedPoints.get(placement);

        adPoint.showAd();

        return true;
    }

    public void rewardedShowSuccessful(Map<String, Object> params) {
        this.increaseStatisticInteger("ad.rewarded.show", 1);

        this.pythonCall("onAndroidAdServiceRewardedShowSuccessful", params);
    }

    public void rewardedShowFailed(Map<String, Object> params) {
        this.pythonCall("onAndroidAdServiceRewardedShowFailed", params);
    }

    public void rewardedRevenuePaid(Map<String, Object> params) {
        double revenue = MengineUtils.getMapDouble(params, "revenue");
        this.increaseStatisticDouble("ad.rewarded.revenue", revenue);
        this.increaseStatisticDouble("ad.total.revenue", revenue);

        this.pythonCall("onAndroidAdServiceRewardedRevenuePaid", params);
    }

    public void rewardedUserRewarded(Map<String, Object> params) {
        this.increaseStatisticInteger("ad.rewarded.rewarded", 1);

        this.pythonCall("onAndroidAdServiceRewardedUserRewarded", params);
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
        if (m_adAppOpenPoints.containsKey(placement) == false) {
            this.logError("ad appopen point '%s' not found", placement);

            return false;
        }

        if (m_adProvider == null) {
            return false;
        }

        MengineApplication application = this.getMengineApplication();

        MengineAdAppOpenPoint adPoint = m_adAppOpenPoints.get(placement);

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
        if (m_adAppOpenPoints.containsKey(placement) == false) {
            this.logError("ad appopen point '%s' not found", placement);

            return false;
        }

        if (m_adProvider == null) {
            return false;
        }

        if (m_adProvider.showAppOpen(placement) == false) {
            return false;
        }

        m_lastShowAppOpen = MengineUtils.getTimestamp();
        m_countShowAppOpen += 1;

        MengineAdAppOpenPoint adPoint = m_adAppOpenPoints.get(placement);

        adPoint.showAd();

        return true;
    }

    public void appOpenShowSuccessful(Map<String, Object> params) {
        this.increaseStatisticInteger("ad.appopen.show", 1);

        this.pythonCall("onAndroidAdServiceAppOpenShowSuccessful", params);
    }

    public void appOpenShowFailed(Map<String, Object> params) {
        this.pythonCall("onAndroidAdServiceAppOpenShowFailed", params);
    }

    public void appOpenRevenuePaid(Map<String, Object> params) {
        double revenue = MengineUtils.getMapDouble(params, "revenue");
        this.increaseStatisticDouble("ad.appopen.revenue", revenue);
        this.increaseStatisticDouble("ad.total.revenue", revenue);

        this.pythonCall("onAndroidAdServiceAppOpenRevenuePaid", params);
    }
}