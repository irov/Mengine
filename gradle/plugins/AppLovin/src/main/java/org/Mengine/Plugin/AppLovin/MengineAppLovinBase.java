package org.Mengine.Plugin.AppLovin;

import android.os.Bundle;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAnalyticsEventBuilder;
import org.Mengine.Base.MengineLog;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdWaterfallInfo;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxMediatedNetworkInfo;
import com.applovin.mediation.MaxNetworkResponseInfo;

import java.util.List;

public class MengineAppLovinBase {
    protected MengineAppLovinPlugin m_plugin;

    public MengineAppLovinBase(MengineAppLovinPlugin plugin) {
        m_plugin = plugin;
    }

    public void destroy() {
        m_plugin = null;
    }

    protected String getMAAdParams(MaxAd ad) {
        StringBuilder sb = new StringBuilder(512);

        sb.append("{");
        sb.append(String.format("\"format\": \"%s\"", ad.getFormat().getLabel()));
        sb.append(String.format(", \"size\": [%d, %d]", ad.getSize().getWidth(), ad.getSize().getHeight()));
        sb.append(String.format(", \"adUnitIdentifier\": \"%s\"", ad.getAdUnitId()));
        sb.append(String.format(", \"networkName\": \"%s\"", ad.getNetworkName()));
        sb.append(String.format(", \"networkPlacement\": \"%s\"", ad.getNetworkPlacement()));
        sb.append(String.format(", \"revenue\": %f", ad.getRevenue()));
        sb.append(String.format(", \"revenuePrecision\": \"%s\"", ad.getRevenuePrecision()));

        String creativeId = ad.getCreativeId();

        if (creativeId != null) {
            sb.append(String.format(", \"creativeIdentifier\": \"%s\"", creativeId));
        }

        String adReviewCreativeId = ad.getAdReviewCreativeId();

        if (adReviewCreativeId != null) {
            sb.append(String.format(", \"adReviewCreativeIdentifier\": \"%s\"", adReviewCreativeId));
        }

        String placement = ad.getPlacement();

        if (placement != null) {
            sb.append(String.format(", \"placement\": \"%s\"", placement));
        }

        String dspName = ad.getDspName();

        if (dspName != null) {
            sb.append(String.format(", \"DSPName\": \"%s\"", dspName));
        }

        String dspId = ad.getDspId();

        if (dspId != null) {
            sb.append(String.format(", \"DSPIdentifier\": \"%s\"", dspName));
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
        sb.append(String.format("\"code\": %d", code));
        sb.append(String.format(", \"message\": \"%s\"", message));
        sb.append(String.format(", \"mediated_network_error_code\": %d", mediated_network_error_code));
        sb.append(String.format(", \"mediated_network_error_message\": \"%s\"", mediated_network_error_message));
        sb.append(String.format(", \"latency\": %d", latency));
        sb.append("}");

        String params = sb.toString();

        return params;
    }

    protected MengineAnalyticsEventBuilder buildEvent(String name) {
        MengineAnalyticsEventBuilder eventBuilder = m_plugin.buildEvent(name);

        return eventBuilder;
    }

    protected void logMaxAd(String type, String callback, MaxAd ad) {
        StringBuilder sb = new StringBuilder(512);

        sb.append("AppLovin: type: " + type + " callback: " + callback + "\n");

        MaxAdWaterfallInfo waterfall = ad.getWaterfall();

        this.writeMaxAdWaterfallInfo(sb, waterfall);

        sb.setLength(sb.length() - 1); //remove last '\n'

        String message = sb.toString();

        m_plugin.logMessage(message);
    }

    protected void logMaxError(String type, String callback, MaxError error) {
        StringBuilder sb = new StringBuilder(512);

        int errorCode = error.getCode();
        String errorMessage = error.getMessage();
        int mediatedNetworkErrorCode = error.getMediatedNetworkErrorCode();
        String mediatedNetworkErrorMessage = error.getMediatedNetworkErrorMessage();

        sb.append("AppLovin: type: " + type + " callback: " + callback + "\n");
        sb.append("MaxError: code: " + errorCode + " message: " + errorMessage + "\n");
        sb.append("MediatedNetwork: code: " + mediatedNetworkErrorCode + " message: " + mediatedNetworkErrorMessage + "\n");

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

        sb.append("Waterfall Name: " + waterfallName + " and Test Name: " + waterfallTestName + "\n");
        sb.append("Waterfall latency was: " + waterfallLatencyMillis + " milliseconds" + "\n");

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

        sb.append("Network -> " + responseMediatedNetwork +
            "\n...adLoadState: " + responseAdLoadState +
            "\n...latency: " + responseLatencyMillis + " milliseconds" +
            "\n...credentials: " + responseCredentials + "\n");

        MaxError error = networkResponse.getError();

        if (error != null) {
            sb.append("...error: " + error + "\n");
        }
    }
}
