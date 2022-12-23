package org.Mengine.Plugin.AppLovin;

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
