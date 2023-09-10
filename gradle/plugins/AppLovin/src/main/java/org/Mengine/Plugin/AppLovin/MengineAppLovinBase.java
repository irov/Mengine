package org.Mengine.Plugin.AppLovin;

import android.content.Context;
import android.os.Bundle;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdWaterfallInfo;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxMediatedNetworkInfo;
import com.applovin.mediation.MaxNetworkResponseInfo;

import org.Mengine.Base.MengineAnalyticsEventBuilder;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineUtils;

import java.util.List;
import java.util.Locale;

public class MengineAppLovinBase {
    protected MengineAppLovinPlugin m_plugin;

    public MengineAppLovinBase(MengineAppLovinPlugin plugin) {
        m_plugin = plugin;
    }

    public void destroy() {
        m_plugin = null;
    }

    protected int getConectivityStatus() {
        MengineApplication application = m_plugin.getMengineApplication();
        Context context = application.getApplicationContext();
        int status = MengineUtils.getConectivityStatus(context);

        return status;
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

        eventBuilder.addParameterLong("connection", this.getConectivityStatus());

        return eventBuilder;
    }

    protected void logMaxAd(String type, String callback, MaxAd ad) {
        StringBuilder sb = new StringBuilder(512);

        sb.append(String.format(Locale.US, "[%s] callback: %s\n", type, callback));

        MaxAdWaterfallInfo waterfall = ad.getWaterfall();

        this.writeMaxAdWaterfallInfo(sb, waterfall);

        sb.setLength(sb.length() - 1); //remove last '\n'

        String message = sb.toString();

        m_plugin.logMessage(message);
    }

    protected void logMaxError(String type, String callback, String adUnitId, MaxError error) {
        StringBuilder sb = new StringBuilder(512);

        int errorCode = error.getCode();
        String errorMessage = error.getMessage();
        int mediatedNetworkErrorCode = error.getMediatedNetworkErrorCode();
        String mediatedNetworkErrorMessage = error.getMediatedNetworkErrorMessage();

        sb.append(String.format(Locale.US, "[%s] callback: %s\n", type, callback));
        sb.append(String.format(Locale.US, "AdUnitId: %s\n", adUnitId));
        sb.append(String.format(Locale.US, "MaxError: code: %d message: %s\n", errorCode, errorMessage));
        sb.append(String.format(Locale.US, "MediatedNetwork: code: %d message: %s\n", mediatedNetworkErrorCode, mediatedNetworkErrorMessage));

        MaxAdWaterfallInfo waterfall = error.getWaterfall();

        this.writeMaxAdWaterfallInfo(sb, waterfall);

        sb.setLength(sb.length() - 1); //remove last '\n'

        String message = sb.toString();

        m_plugin.logWarning(message);
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

    protected void writeMaxNetworkResponseInfo(StringBuilder sb, MaxNetworkResponseInfo networkResponse) {
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
