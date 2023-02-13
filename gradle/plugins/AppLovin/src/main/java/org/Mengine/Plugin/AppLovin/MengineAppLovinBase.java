package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAnalyticsEventBuilder;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdWaterfallInfo;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxNetworkResponseInfo;

public class MengineAppLovinBase {
    protected MengineAppLovinPlugin m_plugin;

    public MengineAppLovinBase(MengineAppLovinPlugin plugin) throws Exception {
        m_plugin = plugin;
    }

    public void destroy() {
        m_plugin = null;
    }

    public String getMAAdParams(MaxAd ad) {
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

        return sb.toString();
    }

    public String getMaxErrorParams(MaxError error) {
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

        return sb.toString();
    }

    public MengineAnalyticsEventBuilder buildEvent(String name) {
        MengineActivity activity = m_plugin.getActivity();

        MengineAnalyticsEventBuilder eventBuilder = activity.buildEvent(name);

        return eventBuilder;
    }

    public void logMaxAd(String type, String callback, MaxAd ad) {
        m_plugin.logInfo( "AppLovin: type: " + type + " callback: " + callback );

        MaxAdWaterfallInfo waterfall = ad.getWaterfall();

        this.logMaxAdWaterfallInfo(waterfall);
    }

    public void logMaxError(String type, String callback, MaxError error) {
        m_plugin.logInfo( "AppLovin: type: " + type + " callback: " + callback );
        m_plugin.logInfo( "MaxError: code: " + error.getCode() + " message: " + error.getMessage() );
        m_plugin.logInfo( "MediatedNetwork: code" + error.getMediatedNetworkErrorCode() + " message: " + error.getMediatedNetworkErrorMessage() );

        MaxAdWaterfallInfo waterfall = error.getWaterfall();

        this.logMaxAdWaterfallInfo(waterfall);
    }

    public void logMaxAdWaterfallInfo(MaxAdWaterfallInfo waterfall) {
        if (waterfall == null) {
            return;
        }

        m_plugin.logInfo( "Waterfall Name: " + waterfall.getName() + " and Test Name: " + waterfall.getTestName() );
        m_plugin.logInfo( "Waterfall latency was: " + waterfall.getLatencyMillis() + " milliseconds" );

        for (MaxNetworkResponseInfo networkResponse : waterfall.getNetworkResponses()) {
            this.logMaxNetworkResponseInfo(networkResponse);
        }
    }

    public void logMaxNetworkResponseInfo(MaxNetworkResponseInfo networkResponse) {
        String waterfallInfoStr = "Network -> " + networkResponse.getMediatedNetwork() +
            "\n...adLoadState: " + networkResponse.getAdLoadState() +
            "\n...latency: " + networkResponse.getLatencyMillis() + " milliseconds" +
            "\n...credentials: " + networkResponse.getCredentials();

        if (networkResponse.getError() != null) {
            waterfallInfoStr += "\n...error: " + networkResponse.getError();
        }

        m_plugin.logInfo(waterfallInfoStr);
    }
}
