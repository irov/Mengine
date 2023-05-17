package org.Mengine.Plugin.Advertising;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MengineUtils;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;

import com.google.android.gms.ads.identifier.AdvertisingIdClient;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.android.gms.common.GooglePlayServicesNotAvailableException;
import com.google.android.gms.common.GooglePlayServicesRepairableException;

import java.io.IOException;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class MengineAdvertisingPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "Advertising";

    private String m_advertisingId;
    private boolean m_advertisingLimitTrackingEnabled;
    private Future<AdvertisingIdClient.Info> m_advertisingFuture;

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        GoogleApiAvailability apiAvailability = GoogleApiAvailability.getInstance();

        final Context context = application.getApplicationContext();
        final int resultCode = apiAvailability.isGooglePlayServicesAvailable(context);

        if (resultCode == ConnectionResult.SUCCESS) {
            ExecutorService executor = Executors.newSingleThreadExecutor();

            Future<AdvertisingIdClient.Info> future = executor.submit(() -> {
                try {
                    AdvertisingIdClient.Info adInfo = AdvertisingIdClient.getAdvertisingIdInfo(context);

                    return adInfo;
                } catch (IOException e) {
                    MengineAdvertisingPlugin.this.logError("invalid get advertising id info IOException: %s"
                        , e.getLocalizedMessage()
                    );
                } catch (IllegalStateException e) {
                    MengineAdvertisingPlugin.this.logError("invalid get advertising id info IllegalStateException: %s"
                        , e.getLocalizedMessage()
                    );
                } catch (GooglePlayServicesNotAvailableException e) {
                    MengineAdvertisingPlugin.this.logError("invalid get advertising id info GooglePlayServicesNotAvailableException: %s"
                        , e.getLocalizedMessage()
                    );
                } catch (GooglePlayServicesRepairableException e) {
                    MengineAdvertisingPlugin.this.logError("invalid get advertising id info GooglePlayServicesRepairableException: %s"
                        , e.getLocalizedMessage()
                    );
                }

                return null;
            });

            m_advertisingFuture = future;
        }
    }

    public void onMengineInitializeBaseServices(MengineActivity activity) {
        if (m_advertisingFuture == null) {
            return;
        }

        AdvertisingIdClient.Info adInfo = null;

        try {
            adInfo = m_advertisingFuture.get();
        } catch (CancellationException ex) {
            this.logError("invalid get advertising id CancellationException: %s"
                , ex.getLocalizedMessage()
            );
        } catch (InterruptedException ex) {
            this.logError("invalid get advertising id InterruptedException: %s"
                , ex.getLocalizedMessage()
            );
        } catch (ExecutionException ex) {
            this.logError("invalid get advertising id ExecutionException: %s"
                , ex.getLocalizedMessage()
            );
        }

        m_advertisingFuture = null;

        this.postAdInfo(activity, adInfo);
    }

    public void postAdInfo(MengineActivity activity, AdvertisingIdClient.Info adInfo) {
        if (adInfo == null ) {
            m_advertisingId = "00000000-0000-0000-0000-000000000000";
            m_advertisingLimitTrackingEnabled = true;
        } else if( adInfo.isLimitAdTrackingEnabled() == true) {
            m_advertisingId = "00000000-0000-0000-0000-000000000000";
            m_advertisingLimitTrackingEnabled = true;
        } else {
            String advertisingId = adInfo.getId();

            if (advertisingId.equals("00000000-0000-0000-0000-000000000000") == true) {
                m_advertisingId = "00000000-0000-0000-0000-000000000000";
                m_advertisingLimitTrackingEnabled = true;
            } else {
                m_advertisingId = advertisingId;
                m_advertisingLimitTrackingEnabled = false;
            }
        }

        this.logMessage("AdvertisingId: %s limit: %s"
            , m_advertisingId
            , m_advertisingLimitTrackingEnabled == true ? "true" : "false"
        );

        activity.sendEvent(MengineEvent.EVENT_ADVERTISING_ID, m_advertisingId, m_advertisingLimitTrackingEnabled);
    }
}
