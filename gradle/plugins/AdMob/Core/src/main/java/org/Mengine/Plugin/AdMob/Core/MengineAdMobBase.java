package org.Mengine.Plugin.AdMob.Core;

import androidx.annotation.NonNull;
import androidx.annotation.Size;
import androidx.annotation.StringRes;

import com.google.android.gms.ads.AdError;
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.ResponseInfo;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAdResponseInterface;
import org.Mengine.Base.MengineAdService;
import org.Mengine.Base.MengineAnalyticsEventBuilderInterface;
import org.Mengine.Base.MengineFragmentAdRevenue;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MengineParamAdRevenue;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import java.util.Locale;
import java.util.Map;
import java.util.concurrent.TimeUnit;

public class MengineAdMobBase extends AdListener implements MengineAdMobAdInterface {
    protected final MengineAdService m_adService;
    protected final MengineAdMobPluginInterface m_plugin;
    protected final MengineAdFormat m_adFormat;

    protected String m_adUnitId;

    protected int m_enumeratorRequest;
    protected int m_requestId;
    protected int m_requestAttempt;
    protected long m_requestTimestamp;

    public MengineAdMobBase(@NonNull MengineAdService adService, @NonNull MengineAdMobPluginInterface plugin, @NonNull MengineAdFormat adFormat) {
        m_adService = adService;
        m_plugin = plugin;
        m_adFormat = adFormat;

        m_enumeratorRequest = 0;
        m_requestId = 0;
        m_requestAttempt = 0;
        m_requestTimestamp = 0;
    }

    @NonNull
    public MengineAdMobPluginInterface getPlugin() {
        return m_plugin;
    }

    protected void setAdUnitId(@StringRes int METADATA_ADUNITID, @NonNull String configAdUnitId) throws MengineServiceInvalidInitializeException {
        String metadataAdUnitId = m_plugin.getResourceString(METADATA_ADUNITID);

        if (metadataAdUnitId.isEmpty() == true) {
            this.invalidInitialize("meta %s not found"
                , m_plugin.getResourceName(METADATA_ADUNITID)
            );
        }

        String adUnitId = m_plugin.getServiceConfigOptString(configAdUnitId, metadataAdUnitId);

        if (adUnitId == null) {
            this.invalidInitialize("config %s is empty"
                , configAdUnitId
            );
        }

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

    protected String getAdErrorParams(@NonNull AdError error) {
        StringBuilder sb = new StringBuilder(512);

        int code = error.getCode();
        String message = error.getMessage();
        String domain = error.getDomain();

        sb.append("{");
        sb.append(String.format(Locale.US, "\"code\": %d", code));
        sb.append(String.format(Locale.US, ", \"message\": \"%s\"", message));
        sb.append(String.format(Locale.US, ", \"domain\": \"%s\"", domain));

        sb.append("}");

        String params = sb.toString();

        return params;
    }

    protected String getLoadAdErrorParams(@NonNull LoadAdError error) {
        StringBuilder sb = new StringBuilder(512);

        int code = error.getCode();
        String message = error.getMessage();
        String domain = error.getDomain();

        sb.append("{");
        sb.append(String.format(Locale.US, "\"code\": %d", code));
        sb.append(String.format(Locale.US, ", \"message\": \"%s\"", message));
        sb.append(String.format(Locale.US, ", \"domain\": \"%s\"", domain));

        ResponseInfo responseInfo = error.getResponseInfo();
        if (responseInfo != null) {
            String responseId = responseInfo.getResponseId();
            if (responseId != null) {
                sb.append(String.format(Locale.US, ", \"response_id\": \"%s\"", responseId));
            }
        }

        sb.append("}");

        String params = sb.toString();

        return params;
    }

    protected void setState(@NonNull @Size(min = 1L, max = 1024L) String name, Object value) {
        m_plugin.setState(name, value);
    }

    protected MengineAnalyticsEventBuilderInterface buildAdEvent(@Size(min = 1L, max = 40L) String name) {
        long requestTime = this.getRequestTime();

        MengineAnalyticsEventBuilderInterface eventBuilder = m_plugin.buildEvent(name);

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

        this.writeCallback(sb, callback);
        this.writeParams(sb, params);
        this.writeBaseInfo(sb);

        sb.setLength(sb.length() - 1); //remove last ' '

        String message = sb.toString();

        m_plugin.logDebug("%s", message);
    }

    protected void logError(String callback, Exception e) {
        StringBuilder sb = new StringBuilder(1024);

        this.writeCallback(sb, callback);
        this.writeBaseInfo(sb);

        sb.append(String.format(Locale.US, "Exception: %s ", e.getMessage()));

        sb.setLength(sb.length() - 1); //remove last ' '

        String message = sb.toString();

        m_plugin.logError("%s", message);
    }

    protected void logAdError(String callback, @NonNull AdError error) {
        StringBuilder sb = new StringBuilder(1024);

        this.writeCallback(sb, callback);
        this.writeBaseInfo(sb);

        int errorCode = error.getCode();
        String errorMessage = error.getMessage();
        String errorDomain = error.getDomain();

        sb.append(String.format(Locale.US, "AdError: code: %d message: %s domain: %s ", errorCode, errorMessage, errorDomain));

        sb.setLength(sb.length() - 1); //remove last ' '

        String message = sb.toString();

        m_plugin.logDebug("%s", message);
    }

    protected void logLoadAdError(String callback, @NonNull LoadAdError error) {
        StringBuilder sb = new StringBuilder(1024);

        this.writeCallback(sb, callback);
        this.writeBaseInfo(sb);

        int errorCode = error.getCode();
        String errorMessage = error.getMessage();
        String errorDomain = error.getDomain();

        sb.append(String.format(Locale.US, "LoadAdError: code: %d message: %s domain: %s ", errorCode, errorMessage, errorDomain));

        sb.setLength(sb.length() - 1); //remove last ' '

        String message = sb.toString();

        m_plugin.logDebug("%s", message);
    }

    protected void writeParams(StringBuilder sb, Map<String, Object> params) {
        if (params == null) {
            return;
        }

        sb.append(String.format(Locale.US, "%s ", params));
    }

    protected void writeCallback(StringBuilder sb, String callback)
    {
        String label = m_adFormat.getName();

        sb.append(String.format(Locale.US, "[%s] %s ", label, callback));
    }

    protected void writeBaseInfo(StringBuilder sb) {
        long requestTime = this.getRequestTime();

        sb.append(String.format(Locale.US, "AdUnitId: %s ", m_adUnitId));
        sb.append(String.format(Locale.US, "RequestId: %d ", m_requestId));
        sb.append(String.format(Locale.US, "RequestTime: %d ", requestTime));
        sb.append(String.format(Locale.US, "RetryAttempt: %d ", m_requestAttempt));
    }

    protected void revenuePaid(@NonNull ResponseInfo responseInfo, @NonNull MengineAdFormat adFormat, String placement, double revenueValue) {
        MengineAdMediation mediation = MengineAdMediation.ADMEDIATION_ADMOB;
        String networkName = "AdMob";
        String adUnitId = m_adUnitId;

        MengineParamAdRevenue revenue = new MengineParamAdRevenue();
        revenue.ADREVENUE_MEDIATION = mediation;
        revenue.ADREVENUE_FORMAT = adFormat;
        revenue.ADREVENUE_ADUNITID = adUnitId;
        revenue.ADREVENUE_PLACEMENT = placement;
        revenue.ADREVENUE_NETWORK = networkName;
        revenue.ADREVENUE_REVENUE_PRECISION = "publisher_defined";
        revenue.ADREVENUE_REVENUE_VALUE = revenueValue;
        revenue.ADREVENUE_REVENUE_CURRENCY = "USD";

        MengineFragmentAdRevenue.INSTANCE.adRevenue(revenue);

        MengineAdResponseInterface adResponse = m_adService.getAdResponse();

        adResponse.onAdRevenuePaid(mediation, adFormat, placement, revenueValue);
    }

    protected void invalidInitialize(String format, Object ... args) throws MengineServiceInvalidInitializeException {
        String serviceName = m_plugin.getServiceName();

        this.setState("invalid.service", serviceName);

        String message = MengineLog.buildTotalMsg(format, args);

        throw new MengineServiceInvalidInitializeException(message);
    }
}