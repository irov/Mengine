package org.Mengine.Plugin.AppLovin;

import android.os.Bundle;

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
    protected MaxAdFormat m_adFormat;
    protected MengineAppLovinPlugin m_plugin;
    protected String m_adUnitId;

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
            this.loadAd();
        }, delayMillis);
    }

    protected int increaseRequestId() {
        this.m_requestId = this.m_enumeratorRequest++;

        return this.m_requestId;
    }

    protected String getMAAdParams(MaxAd ad) {
        StringBuilder sb = new StringBuilder(512);

        sb.append("{");
        sb.append(String.format(Locale.US, "\"format\": \"%s\"", ad.getFormat().getLabel()));
        sb.append(String.format(Locale.US, ", \"size\": [%d, %d]", ad.getSize().getWidth(), ad.getSize().getHeight()));
        sb.append(String.format(Locale.US, ", \"adUnitIdentifier\": \"%s\"", ad.getAdUnitId()));
        sb.append(String.format(Locale.US, ", \"networkName\": \"%s\"", ad.getNetworkName()));
        sb.append(String.format(Locale.US, ", \"networkPlacement\": \"%s\"", ad.getNetworkPlacement()));
        sb.append(String.format(Locale.US, ", \"revenue\": %f", ad.getRevenue()));
        sb.append(String.format(Locale.US, ", \"revenuePrecision\": \"%s\"", ad.getRevenuePrecision()));

        String creativeId = ad.getCreativeId();

        if (creativeId != null) {
            sb.append(String.format(Locale.US, ", \"creativeIdentifier\": \"%s\"", creativeId));
        }

        String adReviewCreativeId = ad.getAdReviewCreativeId();

        if (adReviewCreativeId != null) {
            sb.append(String.format(Locale.US, ", \"adReviewCreativeIdentifier\": \"%s\"", adReviewCreativeId));
        }

        String placement = ad.getPlacement();

        if (placement != null) {
            sb.append(String.format(Locale.US, ", \"placement\": \"%s\"", placement));
        }

        String dspName = ad.getDspName();

        if (dspName != null) {
            sb.append(String.format(Locale.US, ", \"DSPName\": \"%s\"", dspName));
        }

        String dspId = ad.getDspId();

        if (dspId != null) {
            sb.append(String.format(Locale.US, ", \"DSPIdentifier\": \"%s\"", dspName));
        }

        sb.append("}");

        String params = sb.toString();

        return params;
    }

    protected String getMaxErrorParams(MaxError error) {
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

    protected MengineAnalyticsEventBuilder buildEvent(String name) {
        MengineAnalyticsEventBuilder eventBuilder = m_plugin.buildEvent(name);

        return eventBuilder;
    }

    protected void log(String callback) {
        StringBuilder sb = new StringBuilder(512);

        sb.append(String.format(Locale.US, "[%s] callback: %s\n", m_adFormat.getLabel(), callback));

        this.writeBaseInfo(sb);

        sb.setLength(sb.length() - 1); //remove last '\n'

        String message = sb.toString();

        m_plugin.logMessage(message);
    }

    protected void logError(String callback, Exception e) {
        StringBuilder sb = new StringBuilder(1024);

        sb.append(String.format(Locale.US, "[%s] callback: %s exception: %s\n", m_adFormat.getLabel(), callback, e.getMessage()));

        this.writeBaseInfo(sb);

        sb.setLength(sb.length() - 1); //remove last '\n'

        String message = sb.toString();

        m_plugin.logError(message);
    }

    protected void log(String callback, Map<String, Object> params) {
        StringBuilder sb = new StringBuilder(512);

        sb.append(String.format(Locale.US, "[%s] callback: %s %s\n", m_adFormat.getLabel(), callback, params));

        this.writeBaseInfo(sb);

        sb.setLength(sb.length() - 1); //remove last '\n'

        String message = sb.toString();

        m_plugin.logMessage(message);
    }

    protected void logMaxAd(String callback, MaxAd ad) {
        StringBuilder sb = new StringBuilder(2048);

        sb.append(String.format(Locale.US, "[%s] callback: %s\n", m_adFormat.getLabel(), callback));

        this.writeBaseInfo(sb);
        this.writeMaxAdBaseInfo(sb, ad);

        MaxAdWaterfallInfo waterfall = ad.getWaterfall();

        this.writeMaxAdWaterfallInfo(sb, waterfall);

        sb.setLength(sb.length() - 1); //remove last '\n'

        String message = sb.toString();

        m_plugin.logMessage(message);
    }

    protected void logMaxAd(String callback, MaxAd ad, Map<String, Object> params) {
        StringBuilder sb = new StringBuilder(2048);

        sb.append(String.format(Locale.US, "[%s] callback: %s %s\n", m_adFormat.getLabel(), callback, params));

        this.writeBaseInfo(sb);
        this.writeMaxAdBaseInfo(sb, ad);

        MaxAdWaterfallInfo waterfall = ad.getWaterfall();

        this.writeMaxAdWaterfallInfo(sb, waterfall);

        sb.setLength(sb.length() - 1); //remove last '\n'

        String message = sb.toString();

        m_plugin.logMessage(message);
    }

    protected void logMaxAdReward(String callback, MaxAd ad, MaxReward reward) {
        StringBuilder sb = new StringBuilder(2048);

        sb.append(String.format(Locale.US, "[%s] callback: %s\n", m_adFormat.getLabel(), callback));

        this.writeBaseInfo(sb);
        this.writeMaxAdBaseInfo(sb, ad);

        MaxAdWaterfallInfo waterfall = ad.getWaterfall();

        this.writeBaseInfo(sb);
        this.writeMaxAdWaterfallInfo(sb, waterfall);
        this.writeMaxReward(sb, reward);

        sb.setLength(sb.length() - 1); //remove last '\n'

        String message = sb.toString();

        m_plugin.logMessage(message);
    }

    protected void logMaxError(String callback, MaxError error) {
        StringBuilder sb = new StringBuilder(1024);

        int errorCode = error.getCode();
        String errorMessage = error.getMessage();
        int mediatedNetworkErrorCode = error.getMediatedNetworkErrorCode();
        String mediatedNetworkErrorMessage = error.getMediatedNetworkErrorMessage();

        sb.append(String.format(Locale.US, "[%s] callback: %s\n", m_adFormat.getLabel(), callback));

        this.writeBaseInfo(sb);

        sb.append(String.format(Locale.US, "MaxError: code: %d message: %s\n", errorCode, errorMessage));
        sb.append(String.format(Locale.US, "MediatedNetwork: code: %d message: %s\n", mediatedNetworkErrorCode, mediatedNetworkErrorMessage));

        MaxAdWaterfallInfo waterfall = error.getWaterfall();

        this.writeMaxAdWaterfallInfo(sb, waterfall);

        sb.setLength(sb.length() - 1); //remove last '\n'

        String message = sb.toString();

        m_plugin.logWarning(message);
    }

    protected void writeBaseInfo(StringBuilder sb) {
        sb.append(String.format(Locale.US, "AdUnitId: %s\n", m_adUnitId));
        sb.append(String.format(Locale.US, "RequestId: %d\n", m_requestId));
        sb.append(String.format(Locale.US, "RetryAttempt: %d\n", m_retryAttempt));
    }

    protected void writeMaxAdBaseInfo(StringBuilder sb, MaxAd ad) {
        MaxAdFormat format = ad.getFormat();
        sb.append(String.format(Locale.US, "Format: %s\n", format.getLabel()));

        AppLovinSdkUtils.Size size = ad.getSize();
        sb.append(String.format(Locale.US, "Size: %d x %d\n", size.getWidth(), size.getHeight()));

        String networkName = ad.getNetworkName();
        sb.append(String.format(Locale.US, "NetworkName: %s\n", networkName));

        String networkPlacement = ad.getNetworkPlacement();
        sb.append(String.format(Locale.US, "NetworkPlacement: %s\n", networkPlacement));

        String placement = ad.getPlacement();
        sb.append(String.format(Locale.US, "Placement: %s\n", placement));

        double revenue = ad.getRevenue();
        sb.append(String.format(Locale.US, "Revenue: %f\n", revenue));

        String revenuePrecision = ad.getRevenuePrecision();
        sb.append(String.format(Locale.US, "RevenuePrecision: %s\n", revenuePrecision));

        long requestLatencyMillis = ad.getRequestLatencyMillis();
        sb.append(String.format(Locale.US, "RequestLatencyMillis: %d\n", requestLatencyMillis));
    }

    protected void writeMaxAdWaterfallInfo(StringBuilder sb, MaxAdWaterfallInfo waterfall) {
        if (waterfall == null) {
            return;
        }

        String waterfallName = waterfall.getName();
        String waterfallTestName = waterfall.getTestName();
        long waterfallLatencyMillis = waterfall.getLatencyMillis();

        sb.append(String.format(Locale.US, "Waterfall Name: %s and Test Name: %s\n", waterfallName, waterfallTestName));
        sb.append(String.format(Locale.US, "Waterfall latency was: %d milliseconds\n", waterfallLatencyMillis));

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

        sb.append(String.format(Locale.US, "Reward Label: %s Amount: %d\n", label, amount));
    }

    protected void writeMaxNetworkResponseInfo(StringBuilder sb, MaxNetworkResponseInfo networkResponse) {
        if (networkResponse == null) {
            return;
        }

        MaxMediatedNetworkInfo responseMediatedNetwork = networkResponse.getMediatedNetwork();
        MaxNetworkResponseInfo.AdLoadState responseAdLoadState = networkResponse.getAdLoadState();
        long responseLatencyMillis = networkResponse.getLatencyMillis();
        Bundle responseCredentials = networkResponse.getCredentials();

        sb.append(String.format(Locale.US, "Network -> %s\n", responseMediatedNetwork));
        sb.append(String.format(Locale.US, "...adLoadState: %s\n", responseAdLoadState));
        sb.append(String.format(Locale.US, "...latency: %d milliseconds\n", responseLatencyMillis));
        sb.append(String.format(Locale.US, "...credentials: %s\n", responseCredentials));

        MaxError error = networkResponse.getError();

        if (error != null) {
            sb.append(String.format(Locale.US, "...error: %s\n", error));
        }
    }
}
