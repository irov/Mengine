package org.Mengine.Plugin.AppLovin.Core;

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
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAdResponseInterface;
import org.Mengine.Base.MengineAdService;
import org.Mengine.Base.MengineAnalyticsEventBuilder;
import org.Mengine.Base.MengineFragmentAdRevenue;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MengineParamAdRevenue;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.concurrent.TimeUnit;

public class MengineAppLovinBase implements MengineAppLovinAdInterface {
    protected final MengineAppLovinPluginInterface m_plugin;
    protected final MaxAdFormat m_adFormat;
    protected final MengineAdResponseInterface m_adResponse;

    protected String m_adUnitId;

    protected int m_enumeratorRequest;
    protected int m_requestId;
    protected int m_requestAttempt;
    protected long m_requestTimestamp;

    public MengineAppLovinBase(@NonNull MengineAppLovinPluginInterface plugin, MaxAdFormat adFormat) {
        m_plugin = plugin;
        m_adFormat = adFormat;

        MengineAdService adService = plugin.getService(MengineAdService.class);

        m_adResponse = adService.getAdResponse();

        m_enumeratorRequest = 0;
        m_requestId = 0;
        m_requestAttempt = 0;
        m_requestTimestamp = 0;
    }

    @NonNull
    public MengineAppLovinPluginInterface getPlugin() {
        return m_plugin;
    }

    protected void setAdUnitId(@NonNull String adUnitId) {
        m_adUnitId = adUnitId;
    }

    @Override
    public String getAdUnitId() {
        return m_adUnitId;
    }

    @Override
    public void onActivityCreate(@NonNull MengineActivity activity) {
        //Empty
    }

    @Override
    public void onActivityDestroy(@NonNull MengineActivity activity) {
        //Empty
    }

    public void firstLoadAd(@NonNull MengineAppLovinMediationLoadInterface loadCallback) {
        final List<MengineAppLovinMediationInterface> mediations = m_plugin.getMediations();

        if (mediations.isEmpty() == false) {
            final List<MengineAppLovinMediationInterface> loadMediations = List.copyOf(mediations);

            MengineAppLovinMediationLoadedCallback adLoadCallback = new MengineAppLovinMediationLoadedCallback() {
                @Override
                public void onAdLoaded(MengineAppLovinMediationInterface mediation) {
                    this.onAdLoadComplete(mediation);
                }

                @Override
                public void onAdLoadFailed(@NonNull MengineAppLovinMediationInterface mediation, @NonNull Exception exception) {
                    this.onAdLoadComplete(mediation);
                }

                private void onAdLoadComplete(@NonNull MengineAppLovinMediationInterface mediation) {
                    loadMediations.remove(mediation);

                    if (loadMediations.isEmpty()) {
                        MengineUtils.performOnMainThread(() -> {
                            MengineAppLovinBase.this.loadAd();
                        });
                    }
                }
            };

            for (MengineAppLovinMediationInterface mediation : mediations) {
                try {
                    loadCallback.onAdLoad(mediation, adLoadCallback);
                } catch (final MengineServiceInvalidInitializeException e) {
                    m_plugin.logException(e, Map.of());

                    adLoadCallback.onAdLoadFailed(mediation, e);
                }
            }
        } else {
            MengineUtils.performOnMainThread(() -> {
                this.loadAd();
            });
        }
    }

    public void loadAd() {
        // Empty
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

        m_plugin.logInfo("%s", message);
    }

    protected void logError(String callback, Exception e) {
        StringBuilder sb = new StringBuilder(1024);

        String label = m_adFormat.getLabel();

        sb.append(String.format(Locale.US, "[%s] %s exception: %s ", label, callback, e.getMessage()));

        this.writeBaseInfo(sb);

        sb.setLength(sb.length() - 1); //remove last ' '

        String message = sb.toString();

        m_plugin.logError("%s", message);
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

        m_plugin.logInfo("%s", message);
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

        m_plugin.logInfo("%s", message);
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

        m_plugin.logInfo("%s", message);
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

    private static MengineAdFormat getAdFormat(MaxAdFormat format) {
        if (format == MaxAdFormat.BANNER) {
            return MengineAdFormat.ADFORMAT_BANNER;
        } else if (format == MaxAdFormat.MREC) {
            return MengineAdFormat.ADFORMAT_MREC;
        } else if (format == MaxAdFormat.LEADER) {
            return MengineAdFormat.ADFORMAT_LEADER;
        } else if (format == MaxAdFormat.INTERSTITIAL) {
            return MengineAdFormat.ADFORMAT_INTERSTITIAL;
        } else if (format == MaxAdFormat.APP_OPEN) {
            return MengineAdFormat.ADFORMAT_APPOPEN;
        } else if (format == MaxAdFormat.REWARDED) {
            return MengineAdFormat.ADFORMAT_REWARDED;
        } else if (format == MaxAdFormat.NATIVE) {
            return MengineAdFormat.ADFORMAT_NATIVE;
        }

        return MengineAdFormat.ADFORMAT_UNKNOWN;
    }

    protected void revenuePaid(MaxAd ad) {
        MengineAdMediation mediation = MengineAdMediation.ADMEDIATION_APPLOVINMAX;
        String networkName = ad.getNetworkName();
        MaxAdFormat format = ad.getFormat();
        MengineAdFormat adFormat = MengineAppLovinBase.getAdFormat(format);
        String adUnitId = ad.getAdUnitId();
        String placement = ad.getPlacement();
        double revenueValue = ad.getRevenue();
        String revenuePrecision = ad.getRevenuePrecision();

        MengineParamAdRevenue revenue = new MengineParamAdRevenue();
        revenue.ADREVENUE_MEDIATION = mediation;
        revenue.ADREVENUE_FORMAT = adFormat;
        revenue.ADREVENUE_ADUNITID = adUnitId;
        revenue.ADREVENUE_PLACEMENT = placement;
        revenue.ADREVENUE_NETWORK = networkName;
        revenue.ADREVENUE_REVENUE_PRECISION = revenuePrecision;
        revenue.ADREVENUE_REVENUE_VALUE = revenueValue;
        revenue.ADREVENUE_REVENUE_CURRENCY = "USD";

        MengineFragmentAdRevenue.INSTANCE.adRevenue(revenue);

        m_adResponse.onAdRevenuePaid(mediation, adFormat, placement, revenueValue);
    }

    protected void invalidInitialize(String format, Object ... args) throws MengineServiceInvalidInitializeException {
        String serviceName = m_plugin.getServiceName();

        this.setState("invalid.service", serviceName);

        String message = MengineLog.buildTotalMsg(format, args);

        throw new MengineServiceInvalidInitializeException(message);
    }
}
