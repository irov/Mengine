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
import org.Mengine.Base.MengineListenerTransparencyConsent;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineParamTransparencyConsent;
import org.Mengine.Base.MengineUtils;
import org.Mengine.Plugin.GoogleService.MengineGoogleServicePlugin;

import java.io.IOException;
import java.util.Objects;

public class MengineGoogleAdvertisingPlugin extends MengineService implements MengineListenerApplication, MengineListenerTransparencyConsent {
    public static final String SERVICE_NAME = "Advertising";
    public static final int SAVE_VERSION = 1;

    protected String m_advertisingId;
    protected boolean m_advertisingLimitTrackingEnabled = false;
    protected boolean m_advertisingLimitTrackingFetch = false;

    protected final Object m_synchronizationAdvertising = new Object();

    private Thread m_advertisingThread;
    private long m_advertisingRequestId = 0L;
    private boolean m_advertisingConsentGranted = false;

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

        synchronized (m_synchronizationAdvertising) {
            bundle.putString("advertisingId", m_advertisingId);
            bundle.putBoolean("advertisingLimitTrackingEnabled", m_advertisingLimitTrackingEnabled);
            bundle.putBoolean("advertisingLimitTrackingFetch", m_advertisingLimitTrackingFetch);
        }

        return bundle;
    }

    @Override
    public void onLoad(@NonNull MengineApplication application, @NonNull Bundle bundle) {
        int version = bundle.getInt("version", 0);

        synchronized (m_synchronizationAdvertising) {
            m_advertisingId = bundle.getString("advertisingId", MengineFragmentAdvertisingId.LIMIT_ADVERTISING_ID);
            m_advertisingLimitTrackingEnabled = bundle.getBoolean("advertisingLimitTrackingEnabled", true);
            m_advertisingLimitTrackingFetch = bundle.getBoolean("advertisingLimitTrackingFetch", false);
        }
    }

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        if (application.hasTransparencyConsentProvider() == true) {
            this.logInfo("AdvertisingId waiting for transparency consent");

            this.updateAdvertisingConsent(application, false);

            return;
        }

        MengineParamTransparencyConsent tcParam = application.makeTransparencyConsentParam();
        boolean consentAdStorage = tcParam.getConsentAdStorage();

        this.updateAdvertisingConsent(application, consentAdStorage);
    }

    @Override
    public void onMengineTransparencyConsent(@NonNull MengineApplication application, @NonNull MengineParamTransparencyConsent tcParam) {
        boolean consentAdStorage = tcParam.getConsentAdStorage();

        this.updateAdvertisingConsent(application, consentAdStorage);
    }

    private void updateAdvertisingConsent(@NonNull MengineApplication application, boolean consentAdStorage) {
        synchronized (m_synchronizationAdvertising) {
            m_advertisingConsentGranted = consentAdStorage;
        }

        if (consentAdStorage == false) {
            this.logInfo("AdvertisingId disabled by consent ad storage");

            synchronized (m_synchronizationAdvertising) {
                m_advertisingRequestId += 1L;

                if (m_advertisingThread != null) {
                    m_advertisingThread.interrupt();
                    m_advertisingThread = null;
                }

                m_advertisingId = MengineFragmentAdvertisingId.LIMIT_ADVERTISING_ID;
                m_advertisingLimitTrackingEnabled = true;
                m_advertisingLimitTrackingFetch = true;
            }

            MengineFragmentAdvertisingId.INSTANCE.setAdvertisingId(m_advertisingId, m_advertisingLimitTrackingEnabled);

            return;
        }

        this.requestAdvertisingId(application);
    }

    private void requestAdvertisingId(@NonNull MengineApplication application) {
        final long requestId;

        synchronized (m_synchronizationAdvertising) {
            if (m_advertisingLimitTrackingFetch == true) {
                this.logInfo("AdvertisingId: %s limit: %s"
                    , MengineUtils.getRedactedValue(m_advertisingId)
                    , m_advertisingLimitTrackingEnabled == true ? "true" : "false"
                );

                MengineFragmentAdvertisingId.INSTANCE.setAdvertisingId(m_advertisingId, m_advertisingLimitTrackingEnabled);
            }

            if (m_advertisingThread != null) {
                return;
            }

            m_advertisingRequestId += 1L;
            requestId = m_advertisingRequestId;
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

            this.postAdInfo(requestId, adInfo);
        };

        synchronized (m_synchronizationAdvertising) {
            if (requestId != m_advertisingRequestId || m_advertisingConsentGranted == false || m_advertisingThread != null) {
                return;
            }

            m_advertisingThread = new Thread(task, "MNGGAID");
            m_advertisingThread.start();
        }
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        synchronized (m_synchronizationAdvertising) {
            m_advertisingConsentGranted = false;
            m_advertisingRequestId += 1L;

            if (m_advertisingThread != null) {
                m_advertisingThread.interrupt();
                m_advertisingThread = null;
            }
        }
    }

    private void postAdInfo(long requestId, AdvertisingIdClient.Info adInfo) {
        synchronized (m_synchronizationAdvertising) {
            if (requestId != m_advertisingRequestId || m_advertisingConsentGranted == false) {
                return;
            }

            String newAdvertisingId;
            boolean newAdvertisingLimitTrackingEnabled;

            if (adInfo == null) {
                newAdvertisingId = MengineFragmentAdvertisingId.LIMIT_ADVERTISING_ID;
                newAdvertisingLimitTrackingEnabled = true;
            } else if (adInfo.isLimitAdTrackingEnabled() == true) {
                newAdvertisingId = MengineFragmentAdvertisingId.LIMIT_ADVERTISING_ID;
                newAdvertisingLimitTrackingEnabled = true;
            } else {
                String adInfoAdvertisingId = adInfo.getId();

                if (Objects.equals(adInfoAdvertisingId, MengineFragmentAdvertisingId.LIMIT_ADVERTISING_ID) == true) {
                    newAdvertisingId = MengineFragmentAdvertisingId.LIMIT_ADVERTISING_ID;
                    newAdvertisingLimitTrackingEnabled = true;
                } else {
                    newAdvertisingId = adInfoAdvertisingId;
                    newAdvertisingLimitTrackingEnabled = false;
                }
            }

            m_advertisingLimitTrackingFetch = true;

            m_advertisingThread = null;

            if (Objects.equals(m_advertisingId, newAdvertisingId) == true && m_advertisingLimitTrackingEnabled == newAdvertisingLimitTrackingEnabled) {
                return;
            }

            m_advertisingId = newAdvertisingId;
            m_advertisingLimitTrackingEnabled = newAdvertisingLimitTrackingEnabled;
        }

        MengineUtils.performOnMainThread(() -> {
            this.logInfo("AdvertisingId: %s limit: %s"
                , MengineUtils.getRedactedValue(m_advertisingId)
                , m_advertisingLimitTrackingEnabled == true ? "true" : "false"
            );

            MengineFragmentAdvertisingId.INSTANCE.setAdvertisingId(m_advertisingId, m_advertisingLimitTrackingEnabled);
        });
    }
}
