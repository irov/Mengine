package org.Mengine.Plugin.AppLovin;

import android.os.Bundle;

import androidx.annotation.NonNull;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdWaterfallInfo;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxMediatedNetworkInfo;
import com.applovin.mediation.MaxNetworkResponseInfo;
import com.applovin.mediation.MaxReward;
import com.applovin.sdk.AppLovinSdkUtils;

import org.Mengine.Base.MengineAnalyticsEventBuilder;
import org.Mengine.Base.MengineUtils;

import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.concurrent.TimeUnit;

public class MengineAppLovinBase {
    protected final MaxAdFormat m_adFormat;
    protected MengineAppLovinPlugin m_plugin;
    protected final String m_adUnitId;

    protected int m_retryAttempt;

    protected int m_enumeratorRequest;
    protected int m_requestId;

    public MengineAppLovinBase(MaxAdFormat adFormat, MengineAppLovinPlugin plugin, String adUnitId) {
        m_adFormat = adFormat;
        m_plugin = plugin;
        m_adUnitId = adUnitId;

        m_retryAttempt = 0;
        m_enumeratorRequest = 0;
        m_requestId = 0;
    }

    public void destroy() {
        m_plugin = null;
    }

    public void loadAd() {
        //Empty
    }

    protected void retryLoadAd() {
        this.m_retryAttempt++;

        long duration = (long) Math.pow(2, Math.min(6, m_retryAttempt));
        long delayMillis = TimeUnit.SECONDS.toMillis(duration);

        MengineUtils.performOnMainThreadDelayed(() -> {
            MengineAppLovinBase.this.loadAd();
        }, delayMillis);
    }

    protected int increaseRequestId() {
        this.m_requestId = this.m_enumeratorRequest++;

        return this.m_requestId;
    }

    protected String getMAAdParams(@NonNull MaxAd ad) {
        StringBuilder sb = new StringBuilder(512);

        MaxAdFormat format = ad.getFormat();
        String formatLabel = format.getLabel();
        AppLovinSdkUtils.Size size = ad.getSize();
        int sizeWidth = size.getWidth();
        int sizeHeight = size.getHeight();
        String adUnitId = ad.getAdUnitId();
        String networkName = ad.getNetworkName();
        String networkPlacement = ad.getNetworkPlacement();
        double revenue = ad.getRevenue();
        String revenuePrecision = ad.getRevenuePrecision();

        sb.append("{");
        sb.append(String.format(Locale.US, "\"format\": \"%s\"", formatLabel));
        sb.append(String.format(Locale.US, ", \"size\": [%d, %d]", sizeWidth, sizeHeight));
        sb.append(String.format(Locale.US, ", \"ad_unit_id\": \"%s\"", adUnitId));
        sb.append(String.format(Locale.US, ", \"network_name\": \"%s\"", networkName));
        sb.append(String.format(Locale.US, ", \"network_placement\": \"%s\"", networkPlacement));
        sb.append(String.format(Locale.US, ", \"revenue\": %f", revenue));
        sb.append(String.format(Locale.US, ", \"revenue_precision\": \"%s\"", revenuePrecision));

        String creativeId = ad.getCreativeId();

        if (creativeId != null) {
            sb.append(String.format(Locale.US, ", \"creative_identifier\": \"%s\"", creativeId));
        }

        String adReviewCreativeId = ad.getAdReviewCreativeId();

        if (adReviewCreativeId != null) {
            sb.append(String.format(Locale.US, ", \"ad_review_creative_id\": \"%s\"", adReviewCreativeId));
        }

        String placement = ad.getPlacement();

        if (placement != null) {
            sb.append(String.format(Locale.US, ", \"placement\": \"%s\"", placement));
        }

        String dspName = ad.getDspName();

        if (dspName != null) {
            sb.append(String.format(Locale.US, ", \"dsp_name\": \"%s\"", dspName));
        }

        String dspId = ad.getDspId();

        if (dspId != null) {
            sb.append(String.format(Locale.US, ", \"dsp_id\": \"%s\"", dspId));
        }

        sb.append("}");

        String params = sb.toString();

        return params;
    }

    protected String getMaxErrorParams(@NonNull MaxError error) {
        StringBuilder sb = new StringBuilder(512);

        int code = error.getCode();
        String message = error.getMessage();
        int mediated_network_error_code = error.getMediatedNetworkErrorCode();
        String mediated_network_error_message = error.getMediatedNetworkErrorMessage();
        long latency = error.getRequestLatencyMillis();

        sb.append("{");
        sb.append(String.format(Locale.US, "\"code\": %d", code));
        sb.append(String.format(Locale.US, ", \"message\": \"%s\"", message));
        sb.append(String.format(Locale.US, ", \"mediated_network_error_code\": %d", mediated_network_error_code));
        sb.append(String.format(Locale.US, ", \"mediated_network_error_message\": \"%s\"", mediated_network_error_message));
        sb.append(String.format(Locale.US, ", \"latency\": %d", latency));
        sb.append("}");

        String params = sb.toString();

        return params;
    }

    protected MengineAnalyticsEventBuilder buildAdEvent(String name) {
        MengineAnalyticsEventBuilder eventBuilder = m_plugin.buildEvent(name);

        eventBuilder.addParameterString("ad_unit_id", m_adUnitId);
        eventBuilder.addParameterLong("request_id", m_requestId);
        eventBuilder.addParameterLong("attempt", m_retryAttempt);

        return eventBuilder;
    }

    protected void log(String callback) {
        this.log(callback, null);
    }

    protected void log(String callback, Map<String, Object> params) {
        StringBuilder sb = new StringBuilder(512);

        String label = m_adFormat.getLabel();

        sb.append(String.format(Locale.US, "[%s] %s ", label, callback));

        this.writeParams(sb, params);
        this.writeBaseInfo(sb);

        sb.setLength(sb.length() - 1); //remove last ' '

        String message = sb.toString();

        m_plugin.logMessage(message);
    }

    protected void logError(String callback, Exception e) {
        StringBuilder sb = new StringBuilder(1024);

        String label = m_adFormat.getLabel();

        sb.append(String.format(Locale.US, "[%s] %s exception: %s ", label, callback, e.getMessage()));

        this.writeBaseInfo(sb);

        sb.setLength(sb.length() - 1); //remove last ' '

        String message = sb.toString();

        m_plugin.logError(message);
    }

    protected void logMaxAd(String callback, @NonNull MaxAd ad) {
        this.logMaxAd(callback, ad, null);
    }

    protected void logMaxAd(String callback, @NonNull MaxAd ad, Map<String, Object> params) {
        StringBuilder sb = new StringBuilder(2048);

        String label = m_adFormat.getLabel();

        sb.append(String.format(Locale.US, "[%s] %s ", label, callback));

        this.writeParams(sb, params);
        this.writeBaseInfo(sb);
        this.writeMaxAdBaseInfo(sb, ad);

        MaxAdWaterfallInfo waterfall = ad.getWaterfall();

        this.writeMaxAdWaterfallInfo(sb, waterfall);

        sb.setLength(sb.length() - 1); //remove last ' '

        String message = sb.toString();

        m_plugin.logMessage(message);
    }

    protected void logMaxAdReward(String callback, @NonNull MaxAd ad, MaxReward reward) {
        StringBuilder sb = new StringBuilder(2048);

        String label = m_adFormat.getLabel();

        sb.append(String.format(Locale.US, "[%s] %s ", label, callback));

        this.writeBaseInfo(sb);
        this.writeMaxAdBaseInfo(sb, ad);

        MaxAdWaterfallInfo waterfall = ad.getWaterfall();

        this.writeBaseInfo(sb);
        this.writeMaxAdWaterfallInfo(sb, waterfall);
        this.writeMaxReward(sb, reward);

        sb.setLength(sb.length() - 1); //remove last ' '

        String message = sb.toString();

        m_plugin.logMessage(message);
    }

    protected void logMaxError(String callback, @NonNull MaxError error) {
        StringBuilder sb = new StringBuilder(1024);

        String label = m_adFormat.getLabel();

        sb.append(String.format(Locale.US, "[%s] %s ", label, callback));

        this.writeBaseInfo(sb);

        int errorCode = error.getCode();
        String errorMessage = error.getMessage();
        int mediatedNetworkErrorCode = error.getMediatedNetworkErrorCode();
        String mediatedNetworkErrorMessage = error.getMediatedNetworkErrorMessage();

        sb.append(String.format(Locale.US, "MaxError: code: %d message: %s ", errorCode, errorMessage));
        sb.append(String.format(Locale.US, "MediatedNetwork: code: %d message: %s ", mediatedNetworkErrorCode, mediatedNetworkErrorMessage));

        MaxAdWaterfallInfo waterfall = error.getWaterfall();

        this.writeMaxAdWaterfallInfo(sb, waterfall);

        sb.setLength(sb.length() - 1); //remove last ' '

        String message = sb.toString();

        m_plugin.logWarning(message);
    }

    protected void writeParams(StringBuilder sb, Map<String, Object> params) {
        if (params == null) {
            return;
        }

        sb.append(String.format(Locale.US, "%s ", params));
    }

    protected void writeBaseInfo(StringBuilder sb) {
        sb.append(String.format(Locale.US, "AdUnitId: %s ", m_adUnitId));
        sb.append(String.format(Locale.US, "RequestId: %d ", m_requestId));
        sb.append(String.format(Locale.US, "RetryAttempt: %d ", m_retryAttempt));
    }

    protected void writeMaxAdBaseInfo(StringBuilder sb, @NonNull MaxAd ad) {
        sb.append("{Ad} -> ");

        MaxAdFormat format = ad.getFormat();
        String formatLabel = format.getLabel();
        sb.append(String.format(Locale.US, "Format: %s ", formatLabel));

        AppLovinSdkUtils.Size size = ad.getSize();
        int sizeWidth = size.getWidth();
        int sizeHeight = size.getHeight();
        sb.append(String.format(Locale.US, "Size: %d x %d ", sizeWidth, sizeHeight));

        String networkName = ad.getNetworkName();
        sb.append(String.format(Locale.US, "NetworkName: %s ", networkName));

        String networkPlacement = ad.getNetworkPlacement();
        sb.append(String.format(Locale.US, "NetworkPlacement: %s ", networkPlacement));

        String placement = ad.getPlacement();
        sb.append(String.format(Locale.US, "Placement: %s ", placement));

        double revenue = ad.getRevenue();
        sb.append(String.format(Locale.US, "Revenue: %f ", revenue));

        String revenuePrecision = ad.getRevenuePrecision();
        sb.append(String.format(Locale.US, "RevenuePrecision: %s ", revenuePrecision));

        long requestLatencyMillis = ad.getRequestLatencyMillis();
        sb.append(String.format(Locale.US, "RequestLatencyMillis: %d ", requestLatencyMillis));
    }

    protected void writeMaxAdWaterfallInfo(StringBuilder sb, MaxAdWaterfallInfo waterfall) {
        if (waterfall == null) {
            return;
        }

        String waterfallName = waterfall.getName();
        String waterfallTestName = waterfall.getTestName();
        long waterfallLatencyMillis = waterfall.getLatencyMillis();

        sb.append("{Waterfall} -> ");
        sb.append(String.format(Locale.US, "Name: %s ", waterfallName));
        sb.append(String.format(Locale.US, "TestName: %s ", waterfallTestName));
        sb.append(String.format(Locale.US, "Latency: %d ", waterfallLatencyMillis));

        List<MaxNetworkResponseInfo> responses = waterfall.getNetworkResponses();

        for (MaxNetworkResponseInfo networkResponse : responses) {
            this.writeMaxNetworkResponseInfo(sb, networkResponse);
        }
    }

    protected void writeMaxReward(StringBuilder sb, MaxReward reward) {
        if (reward == null) {
            return;
        }

        String label = reward.getLabel();
        int amount = reward.getAmount();

        sb.append("{Reward} -> ");
        sb.append(String.format(Locale.US, "Label: %s Amount: %d ", label, amount));
    }

    protected void writeMaxNetworkResponseInfo(StringBuilder sb, MaxNetworkResponseInfo networkResponse) {
        if (networkResponse == null) {
            return;
        }

        MaxMediatedNetworkInfo responseMediatedNetwork = networkResponse.getMediatedNetwork();
        MaxNetworkResponseInfo.AdLoadState responseAdLoadState = networkResponse.getAdLoadState();
        long responseLatencyMillis = networkResponse.getLatencyMillis();
        Bundle responseCredentials = networkResponse.getCredentials();

        sb.append("{NetworkResponse} -> ");
        sb.append(String.format(Locale.US, "MediatedNetwork: %s ", responseMediatedNetwork));
        sb.append(String.format(Locale.US, "AdLoadState: %s ", responseAdLoadState));
        sb.append(String.format(Locale.US, "LatencyMillis: %d ", responseLatencyMillis));
        sb.append(String.format(Locale.US, "Credentials: %s ", responseCredentials));

        MaxError error = networkResponse.getError();

        if (error != null) {
            sb.append(String.format(Locale.US, "Error: %s ", error));
        }
    }
}
