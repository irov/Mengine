package org.Mengine.Plugin.GoogleAdvertising;

import android.content.Context;
import android.os.Bundle;

import androidx.annotation.NonNull;

import com.google.android.gms.ads.identifier.AdvertisingIdClient;
import com.google.android.gms.common.GooglePlayServicesNotAvailableException;
import com.google.android.gms.common.GooglePlayServicesRepairableException;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineFragmentAdvertisingId;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineParamTransparencyConsent;
import org.Mengine.Base.MengineUtils;
import org.Mengine.Plugin.GoogleService.MengineGoogleServicePlugin;

import java.io.IOException;
import java.util.Objects;

public class MengineGoogleAdvertisingPlugin extends MengineService implements MengineListenerApplication {
    public static final String SERVICE_NAME = "Advertising";
    public static final int SAVE_VERSION = 1;

    private static final String LIMIT_ADVERTISING_ID = "00000000-0000-0000-0000-000000000000";

    protected String m_advertisingId;
    protected boolean m_advertisingLimitTrackingEnabled = false;
    protected boolean m_advertisingLimitTrackingFetch = false;

    protected final Object m_syncronizationAdvertising = new Object();

    private Thread m_advertisingThread;

    @Override
    public boolean onAvailable(@NonNull MengineApplication application) {
        if (MengineGoogleServicePlugin.isGooglePlayServicesAvailable(application) == false) {
            return false;
        }

        return true;
    }

    @Override
    public Bundle onSave(@NonNull MengineApplication application) {
        Bundle bundle = new Bundle();

        bundle.putInt("version", SAVE_VERSION);

        synchronized (m_syncronizationAdvertising) {
            bundle.putString("advertisingId", m_advertisingId);
            bundle.putBoolean("advertisingLimitTrackingEnabled", m_advertisingLimitTrackingEnabled);
            bundle.putBoolean("advertisingLimitTrackingFetch", m_advertisingLimitTrackingFetch);
        }

        return bundle;
    }

    @Override
    public void onLoad(@NonNull MengineApplication application, @NonNull Bundle bundle) {
        int version = bundle.getInt("version", 0);

        synchronized (m_syncronizationAdvertising) {
            m_advertisingId = bundle.getString("advertisingId", LIMIT_ADVERTISING_ID);
            m_advertisingLimitTrackingEnabled = bundle.getBoolean("advertisingLimitTrackingEnabled", true);
            m_advertisingLimitTrackingFetch = bundle.getBoolean("advertisingLimitTrackingFetch", false);
        }
    }

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        MengineParamTransparencyConsent tcParam = application.makeTransparencyConsentParam();

        if (tcParam.getConsentAdStorage() == false) {
            this.logInfo("AdvertisingId disabled by consent ad storage");

            m_advertisingId = LIMIT_ADVERTISING_ID;
            m_advertisingLimitTrackingEnabled = true;
            m_advertisingLimitTrackingFetch = true;

            MengineFragmentAdvertisingId.INSTANCE.setAdvertisingId(m_advertisingId, m_advertisingLimitTrackingEnabled);

            return;
        }

        Runnable task = () -> {
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

            this.postAdInfo(adInfo);
        };

        synchronized (m_syncronizationAdvertising) {
            m_advertisingThread = new Thread(task, "MengineGAID");
            m_advertisingThread.start();
        }
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        synchronized (m_syncronizationAdvertising) {
            if (m_advertisingThread != null) {
                m_advertisingThread.interrupt();
                m_advertisingThread = null;
            }
        }
    }

    private void postAdInfo(AdvertisingIdClient.Info adInfo) {
        synchronized (m_syncronizationAdvertising) {
            if (adInfo == null) {
                m_advertisingId = LIMIT_ADVERTISING_ID;
                m_advertisingLimitTrackingEnabled = true;
            } else if (adInfo.isLimitAdTrackingEnabled() == true) {
                m_advertisingId = LIMIT_ADVERTISING_ID;
                m_advertisingLimitTrackingEnabled = true;
            } else {
                String adInfoAdvertisingId = adInfo.getId();

                if (Objects.equals(adInfoAdvertisingId, LIMIT_ADVERTISING_ID) == true) {
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
            , MengineUtils.getRedactedValue(m_advertisingId)
            , m_advertisingLimitTrackingEnabled == true ? "true" : "false"
        );

        MengineUtils.performOnMainThread(() -> {
            MengineFragmentAdvertisingId.INSTANCE.setAdvertisingId(m_advertisingId, m_advertisingLimitTrackingEnabled);
        });

        synchronized (m_syncronizationAdvertising) {
            m_advertisingThread = null;
        }
    }
}
