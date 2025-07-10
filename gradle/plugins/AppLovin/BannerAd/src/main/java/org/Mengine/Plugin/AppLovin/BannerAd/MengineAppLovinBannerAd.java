package org.Mengine.Plugin.AppLovin.BannerAd;

import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;
import android.graphics.Color;

import androidx.annotation.NonNull;
import androidx.annotation.Size;
import androidx.annotation.StringRes;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxAdReviewListener;
import com.applovin.mediation.MaxAdViewAdListener;
import com.applovin.mediation.MaxAdViewConfiguration;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.ads.MaxAdView;
import com.applovin.sdk.AppLovinSdkUtils;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAnalyticsEventBuilderInterface;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinBannerAdInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinBase;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinNonetBannersInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinPluginInterface;

import java.util.Map;

public class MengineAppLovinBannerAd extends MengineAppLovinBase implements MengineAppLovinBannerAdInterface, MaxAdRequestListener, MaxAdViewAdListener, MaxAdRevenueListener, MaxAdReviewListener {
    public static final @StringRes int METADATA_BANNER_PLACEMENT = R.string.mengine_applovin_banner_placement;
    public static final @StringRes int METADATA_BANNER_ADUNITID = R.string.mengine_applovin_banner_adunitid;

    protected final String m_placement;

    protected MaxAdView m_adView;

    protected volatile boolean m_visible = false;
    protected volatile boolean m_loaded = false;

    public MengineAppLovinBannerAd(@NonNull MengineAppLovinPluginInterface plugin) throws MengineServiceInvalidInitializeException {
        super(plugin, MaxAdFormat.BANNER);

        String MengineAppLovinPlugin_Banner_AdUnitId = plugin.getResourceString(METADATA_BANNER_ADUNITID);

        if (MengineAppLovinPlugin_Banner_AdUnitId.isEmpty() == true) {
            this.invalidInitialize("meta %s is empty"
                , plugin.getResourceName(METADATA_BANNER_ADUNITID)
                , MengineAppLovinPlugin_Banner_AdUnitId
            );
        }

        this.setAdUnitId(MengineAppLovinPlugin_Banner_AdUnitId);

        String MengineAppLovinPlugin_Banner_Placement = plugin.getResourceString(METADATA_BANNER_PLACEMENT);

        m_placement = MengineAppLovinPlugin_Banner_Placement;
    }

    protected AppLovinSdkUtils.Size getBannerSize(@NonNull MengineActivity activity) {
        AppLovinSdkUtils.Size size = MaxAdFormat.BANNER.getAdaptiveSize(activity);

        return size;
    }

    protected MengineAnalyticsEventBuilderInterface buildBannerAdEvent(@Size(min = 1L,max = 40L) String event) {
        MengineAnalyticsEventBuilderInterface builder = this.buildAdEvent("mng_applovin_banner_" + event)
            .addParameterString("placement", m_placement);

        return builder;
    }

    protected void setBannerState(@NonNull String state) {
        this.setState("applovin.banner.state." + m_adUnitId, state);
    }

    public int getWidthPx() {
        MengineActivity activity = m_plugin.getMengineActivity();

        AppLovinSdkUtils.Size size = this.getBannerSize(activity);

        int widthDp = size.getWidth();
        int widthPx = AppLovinSdkUtils.dpToPx(activity, widthDp);

        return widthPx;
    }

    public int getHeightPx() {
        MengineActivity activity = m_plugin.getMengineActivity();

        AppLovinSdkUtils.Size size = this.getBannerSize(activity);

        int heightDp = size.getHeight();
        int heightPx = AppLovinSdkUtils.dpToPx(activity, heightDp);

        return heightPx;
    }

    @Override
    public void onActivityCreate(@NonNull MengineActivity activity) {
        super.onActivityCreate(activity);

        MaxAdViewConfiguration adConfig = MaxAdViewConfiguration.builder()
                .setAdaptiveType(MaxAdViewConfiguration.AdaptiveType.ANCHORED)
                .build();

        MaxAdView adView = new MaxAdView(m_adUnitId, MaxAdFormat.BANNER, adConfig);

        adView.setPlacement(m_placement);

        adView.setListener(this);
        adView.setRequestListener(this);
        adView.setRevenueListener(this);
        adView.setAdReviewListener(this);

        AppLovinSdkUtils.Size size = this.getBannerSize(activity);

        int widthDp = size.getWidth();
        int heightDp = size.getHeight();

        int widthPx = AppLovinSdkUtils.dpToPx(activity, widthDp);
        int heightPx = AppLovinSdkUtils.dpToPx(activity, heightDp);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, heightPx);
        params.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);

        adView.setLayoutParams(params);

        adView.setVisibility(View.GONE);
        adView.setBackgroundColor(Color.TRANSPARENT);

        m_adView = adView;

        this.log("create", Map.of("placement", m_placement, "width", widthDp, "height", heightDp));

        this.setBannerState("init." + m_placement);

        this.firstLoadAd((mediation, callback) -> {
            mediation.loadMediatorBanner(activity, m_plugin, m_adView, callback);
        });

        ViewGroup viewGroup = activity.getContentViewGroup();

        viewGroup.addView(m_adView);
    }

    @Override
    public void onActivityDestroy(@NonNull MengineActivity activity) {
        super.onActivityDestroy(activity);

        if (m_adView != null) {
            m_adView.setListener(null);
            m_adView.setRequestListener(null);
            m_adView.setRevenueListener(null);
            m_adView.setAdReviewListener(null);

            ViewGroup viewGroup = activity.getContentViewGroup();
            viewGroup.removeView(m_adView);

            m_adView.destroy();
            m_adView = null;
        }
    }

    @Override
    public void loadAd() {
        if (m_adView == null) {
            return;
        }

        if (m_plugin.hasOption("applovin.banner.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        this.increaseRequestId();

        this.log("loadAd");

        this.buildBannerAdEvent("load")
            .log();

        this.setBannerState("load");

        try {
            m_adView.loadAd();
        } catch (final Exception e) {
            m_plugin.logError("[Banner] loadAd adUnitId: %s exception: %s"
                , m_adUnitId
                , e.getMessage()
            );

            this.buildBannerAdEvent("load_exception")
                .addParameterException("exception", e)
                .log();

            this.setBannerState("load_exception");

            this.retryLoadAd();
        }
    }

    protected void enableAdView(@NonNull MaxAdView adView) {
        if (adView.getVisibility() == View.VISIBLE) {
            return;
        }

        if (m_plugin.hasOption("applovin.banner.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        m_plugin.logInfo("[Banner] show adView: %s", m_adUnitId);

        adView.setVisibility(View.VISIBLE);
        adView.requestLayout();

        adView.startAutoRefresh();
    }

    protected void disableAdView(@NonNull MaxAdView adView) {
        if (adView.getVisibility() == View.GONE) {
            return;
        }

        if (m_plugin.hasOption("applovin.banner.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        m_plugin.logInfo("[Banner] hide adView: %s", m_adUnitId);

        adView.setVisibility(View.GONE);
        adView.requestLayout();

        adView.setExtraParameter( "allow_pause_auto_refresh_immediately", "true" );
        adView.stopAutoRefresh();
    }

    private void updateVisible() {
        this.log("updateVisible", Map.of("show", m_visible));

        MengineAppLovinNonetBannersInterface nonetBanners = m_plugin.getNonetBanners();

        if (m_visible == true) {
            if (nonetBanners != null) {
                if (m_loaded == true) {
                    nonetBanners.hide();

                    if (m_adView != null) {
                        this.enableAdView(m_adView);
                    }
                } else {
                    nonetBanners.show();
                }
            } else {
                if (m_adView != null) {
                    this.enableAdView(m_adView);
                }
            }
        } else {
            if (m_adView != null) {
                this.disableAdView(m_adView);
            }

            if (nonetBanners != null) {
                nonetBanners.hide();
            }
        }
    }

    public boolean canYouShow() {
        return m_loaded;
    }

    public void show() {
        MengineUtils.performOnMainThread(() -> {
            if (m_visible == true) {
                return;
            }

            m_visible = true;

            this.updateVisible();
        });
    }

    public void hide() {
        MengineUtils.performOnMainThread(() -> {
            if (m_visible == false) {
                return;
            }

            m_visible = false;

            this.updateVisible();
        });
    }

    @Override
    public void onAdRequestStarted(@NonNull String adUnitId) {
        this.increaseRequestId();

        this.log("onAdRequestStarted");

        this.buildBannerAdEvent("request_started")
            .log();

        this.setBannerState("request_started." + m_placement);
    }

    @Override
    public void onAdLoaded(@NonNull MaxAd ad) {
        m_loaded = true;

        this.logMaxAd("onAdLoaded", ad);

        this.buildBannerAdEvent("loaded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        m_requestAttempt = 0;

        this.setBannerState("loaded." + m_placement + "." + ad.getNetworkName());

        if (m_visible == true) {
            MengineAppLovinNonetBannersInterface nonetBanners = m_plugin.getNonetBanners();

            if (nonetBanners != null) {
                this.enableAdView(m_adView);

                nonetBanners.hide();
            }
        }
    }

    @Override
    public void onAdDisplayed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdDisplayed", ad);

        this.buildBannerAdEvent("displayed")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setBannerState("displayed." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdHidden(@NonNull MaxAd ad) {
        this.logMaxAd("onAdHidden", ad);

        this.buildBannerAdEvent("hidden")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setBannerState("hidden." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdClicked(@NonNull MaxAd ad) {
        this.logMaxAd("onAdClicked", ad);

        this.buildBannerAdEvent("clicked")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setBannerState("clicked." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdLoadFailed(@NonNull String adUnitId, @NonNull MaxError error) {
        this.logMaxError("onAdLoadFailed", error);

        int errorCode = error.getCode();

        this.buildBannerAdEvent("load_failed")
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .addParameterLong("error_code", errorCode)
            .log();

        m_requestAttempt++;

        this.setBannerState("load_failed." + m_placement + "." + errorCode);

        if (m_visible == true) {
            MengineAppLovinNonetBannersInterface nonetBanners = m_plugin.getNonetBanners();

            if (nonetBanners != null) {
                this.disableAdView(m_adView);

                nonetBanners.show();
            }
        }
    }

    @Override
    public void onAdDisplayFailed(@NonNull MaxAd ad, @NonNull MaxError error) {
        this.logMaxError("onAdDisplayFailed", error);

        int errorCode = error.getCode();

        this.buildBannerAdEvent("display_failed")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .addParameterJSON("error", this.getMaxErrorParams(error))
            .addParameterLong("error_code", errorCode)
            .log();

        this.setBannerState("display_failed." + m_placement + "." + ad.getNetworkName() + "." + errorCode);
    }

    @Override
    public void onAdExpanded(@NonNull MaxAd ad) {
        this.logMaxAd("onAdExpanded", ad);

        this.buildBannerAdEvent("expanded")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setBannerState("expanded." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdCollapsed(@NonNull MaxAd ad) {
        this.logMaxAd("onAdCollapsed", ad);

        this.buildBannerAdEvent("collapsed")
            .addParameterJSON("ad", this.getMAAdParams(ad))
            .log();

        this.setBannerState("collapsed." + m_placement + "." + ad.getNetworkName());
    }

    @Override
    public void onAdRevenuePaid(@NonNull MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        this.revenuePaid(ad);
    }

    @Override
    public void onCreativeIdGenerated(@NonNull String creativeId, @NonNull MaxAd ad) {
        //ToDo
    }
}
