package org.Mengine.Plugin.Advertising;

import android.content.Context;
import android.os.Bundle;

import androidx.annotation.NonNull;

import com.google.android.gms.ads.identifier.AdvertisingIdClient;
import com.google.android.gms.common.GooglePlayServicesNotAvailableException;
import com.google.android.gms.common.GooglePlayServicesRepairableException;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Plugin.GoogleService.MengineGoogleServicePlugin;

import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class MengineAdvertisingPlugin extends MengineService implements MengineListenerApplication {
    public static final String SERVICE_NAME = "Advertising";
    public static final int SAVE_VERSION = 1;

    private static final String LIMIT_ADVERTISING_ID = "00000000-0000-0000-0000-000000000000";

    protected String m_advertisingId;
    protected boolean m_advertisingLimitTrackingEnabled = false;
    protected boolean m_advertisingLimitTrackingFetch = false;

    private Future<AdvertisingIdClient.Info> m_advertisingFuture;

    @Override
    public boolean onAvailable(@NonNull MengineApplication application) {
        if (MengineGoogleServicePlugin.isGooglePlayServicesAvailable(application) == false) {
            return false;
        }

        return true;
    }

    @Override
    public void onLoad(@NonNull MengineApplication application, @NonNull Bundle bundle) {
        int version = bundle.getInt("version", 0);

        m_advertisingId = bundle.getString("advertisingId", LIMIT_ADVERTISING_ID);
        m_advertisingLimitTrackingEnabled = bundle.getBoolean("advertisingLimitTrackingEnabled", true);
        m_advertisingLimitTrackingFetch = bundle.getBoolean("advertisingLimitTrackingFetch", false);
    }

    @Override
    public Bundle onSave(@NonNull MengineApplication application) {
        Bundle bundle = new Bundle();

        bundle.putInt("version", SAVE_VERSION);

        synchronized (this) {
            bundle.putString("advertisingId", m_advertisingId);
            bundle.putBoolean("advertisingLimitTrackingEnabled", m_advertisingLimitTrackingEnabled);
            bundle.putBoolean("advertisingLimitTrackingFetch", m_advertisingLimitTrackingFetch);
        }

        return bundle;
    }

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        ExecutorService executor = Executors.newSingleThreadExecutor();

        Future<AdvertisingIdClient.Info> future = executor.submit(() -> {
            Context context = application.getApplicationContext();

            AdvertisingIdClient.Info adInfo = null;

            try {
                adInfo = AdvertisingIdClient.getAdvertisingIdInfo(context);
            } catch (final IOException e) {
                this.logError("[ERROR] invalid get advertising id info IOException: %s"
                    , e.getMessage()
                );
            } catch (final IllegalStateException e) {
                this.logError("[ERROR] invalid get advertising id info IllegalStateException: %s"
                    , e.getMessage()
                );
            } catch (final GooglePlayServicesNotAvailableException e) {
                this.logError("[ERROR] invalid get advertising id info GooglePlayServicesNotAvailableException: %s"
                    , e.getMessage()
                );
            } catch (final GooglePlayServicesRepairableException e) {
                this.logError("[ERROR] invalid get advertising id info GooglePlayServicesRepairableException: %s"
                    , e.getMessage()
                );
            }

            this.postAdInfo(application, adInfo);

            return adInfo;
        });

        m_advertisingFuture = future;
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        if (m_advertisingFuture != null) {
            m_advertisingFuture.cancel(true);
            m_advertisingFuture = null;
        }
    }

    private void postAdInfo(MengineApplication application, AdvertisingIdClient.Info adInfo) {
        synchronized (this) {
            if (adInfo == null) {
                m_advertisingId = LIMIT_ADVERTISING_ID;
                m_advertisingLimitTrackingEnabled = true;
            } else if (adInfo.isLimitAdTrackingEnabled() == true) {
                m_advertisingId = LIMIT_ADVERTISING_ID;
                m_advertisingLimitTrackingEnabled = true;
            } else {
                String adInfoAdvertisingId = adInfo.getId();

                if (adInfoAdvertisingId == null || adInfoAdvertisingId.equals(LIMIT_ADVERTISING_ID) == true) {
                    m_advertisingId = LIMIT_ADVERTISING_ID;
                    m_advertisingLimitTrackingEnabled = true;
                } else {
                    m_advertisingId = adInfoAdvertisingId;
                    m_advertisingLimitTrackingEnabled = false;
                }
            }

            m_advertisingLimitTrackingFetch = true;
        }

        this.logInfo("AdvertisingId: %s limit: %s"
            , m_advertisingId
            , m_advertisingLimitTrackingEnabled == true ? "true" : "false"
        );

        application.onMengineAdvertisingId(m_advertisingId, m_advertisingLimitTrackingEnabled);
    }
}
