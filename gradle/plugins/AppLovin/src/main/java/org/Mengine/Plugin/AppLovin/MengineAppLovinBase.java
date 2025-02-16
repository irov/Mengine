package org.Mengine.Plugin.AppLovin;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdWaterfallInfo;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxMediatedNetworkInfo;
import com.applovin.mediation.MaxNetworkResponseInfo;
import com.applovin.mediation.MaxReward;
import com.applovin.sdk.AppLovinSdkUtils;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAnalyticsEventBuilder;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineUtils;

import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.concurrent.TimeUnit;

public class MengineAppLovinBase {
    protected final MengineAppLovinPlugin m_plugin;
    protected final String m_adUnitId;
    protected final MaxAdFormat m_adFormat;

    protected int m_enumeratorRequest;
    protected int m_requestId;
    protected int m_requestAttempt;
    protected long m_requestTimestamp;

    public MengineAppLovinBase(@NonNull MengineAppLovinPlugin plugin, String adUnitId, MaxAdFormat adFormat) {
        m_plugin = plugin;
        m_adUnitId = adUnitId;
        m_adFormat = adFormat;

        m_enumeratorRequest = 0;
        m_requestId = 0;
        m_requestAttempt = 0;
        m_requestTimestamp = 0;
    }

    public boolean isInitialize() {
        return false;
    }

    public void initialize(@NonNull MengineApplication application) {
        //Empty
    }

    public void finalize(@NonNull MengineApplication application) {
        //Empty
    }

    public void onActivityCreate(@NonNull MengineActivity activity) {
        //Empty
    }

    public void onActivityDestroy(@NonNull MengineActivity activity) {
        //Empty
    }

    public void loadAd() {
        //Empty
    }

    protected void retryLoadAd() {
        m_requestAttempt++;

        long duration = (long) Math.pow(2, Math.min(6, m_requestAttempt));
        long delayMillis = TimeUnit.SECONDS.toMillis(duration);

        MengineUtils.performOnMainThreadDelayed(() -> {
            this.loadAd();
        }, delayMillis);
    }

    protected int increaseRequestId() {
        m_requestId = m_enumeratorRequest++;
        m_requestTimestamp = MengineUtils.getTimestamp();

        return m_requestId;
    }

    protected long getRequestTime() {
        long timestamp = MengineUtils.getTimestamp();
        long requestTime = timestamp - m_requestTimestamp;

        return requestTime;
    }

    protected String getMAAdParams(@NonNull MaxAd ad) {
        StringBuilder sb = new StringBuilder(1024);

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

    protected void setState(@NonNull @Size(min = 1L,max = 1024L) String name, Object value) {
        m_plugin.setState(name, value);
    }

    protected MengineAnalyticsEventBuilder buildAdEvent(@Size(min = 1L,max = 40L) String name) {
        long requestTime = this.getRequestTime();

        MengineAnalyticsEventBuilder eventBuilder = m_plugin.buildEvent(name);

        eventBuilder.addParameterString("ad_unit_id", m_adUnitId);
        eventBuilder.addParameterLong("request_id", m_requestId);
        eventBuilder.addParameterLong("request_time", requestTime);
        eventBuilder.addParameterLong("request_attempt", m_requestAttempt);

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

        m_plugin.logMessage("%s", message);
    }

    protected void logError(String callback, Exception e) {
        StringBuilder sb = new StringBuilder(1024);

        String label = m_adFormat.getLabel();

        sb.append(String.format(Locale.US, "[%s] %s exception: %s ", label, callback, e.getMessage()));

        this.writeBaseInfo(sb);

        sb.setLength(sb.length() - 1); //remove last ' '

        String message = sb.toString();

        m_plugin.logMessage("%s", message);
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

        m_plugin.logMessage("%s", message);
    }

    protected void logMaxAdReward(String callback, @NonNull MaxAd ad, MaxReward reward) {
        this.logMaxAdReward(callback, ad, reward, null);
    }

    protected void logMaxAdReward(String callback, @NonNull MaxAd ad, MaxReward reward, Map<String, Object> params) {
        StringBuilder sb = new StringBuilder(2048);

        String label = m_adFormat.getLabel();

        sb.append(String.format(Locale.US, "[%s] %s ", label, callback));

        this.writeParams(sb, params);
        this.writeBaseInfo(sb);
        this.writeMaxAdBaseInfo(sb, ad);

        MaxAdWaterfallInfo waterfall = ad.getWaterfall();

        this.writeBaseInfo(sb);
        this.writeMaxAdWaterfallInfo(sb, waterfall);
        this.writeMaxReward(sb, reward);

        sb.setLength(sb.length() - 1); //remove last ' '

        String message = sb.toString();

        m_plugin.logMessage("%s", message);
    }

    protected void logMaxError(String callback, @NonNull MaxError error) {
        this.logMaxError(callback, error, null);
    }

    protected void logMaxError(String callback, @NonNull MaxError error, Map<String, Object> params) {
        StringBuilder sb = new StringBuilder(1024);

        String label = m_adFormat.getLabel();

        sb.append(String.format(Locale.US, "[%s] %s ", label, callback));

        this.writeParams(sb, params);
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

        m_plugin.logMessage("%s", message);
    }

    protected void writeParams(StringBuilder sb, Map<String, Object> params) {
        if (params == null) {
            return;
        }

        sb.append(String.format(Locale.US, "%s ", params));
    }

    protected void writeBaseInfo(StringBuilder sb) {
        long requestTime = this.getRequestTime();

        sb.append(String.format(Locale.US, "AdUnitId: %s ", m_adUnitId));
        sb.append(String.format(Locale.US, "RequestId: %d ", m_requestId));
        sb.append(String.format(Locale.US, "RequestTime: %d ", requestTime));
        sb.append(String.format(Locale.US, "RetryAttempt: %d ", m_requestAttempt));
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

    protected void pythonCall(@NonNull String method, Object ... params) {
        m_plugin.pythonCall(method, params);
    }
}
