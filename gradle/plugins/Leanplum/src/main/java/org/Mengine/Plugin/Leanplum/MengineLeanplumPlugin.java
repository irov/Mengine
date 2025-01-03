package org.Mengine.Plugin.Leanplum;

import com.leanplum.Leanplum;
import com.leanplum.LeanplumActivityHelper;
import com.leanplum.annotations.Parser;
import com.leanplum.internal.Log;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

public class MengineLeanplumPlugin extends MenginePlugin implements MenginePluginApplicationListener, MenginePluginSessionIdListener {
    public static final String SERVICE_NAME = "Leanplum";

    public static final String METADATA_APP_ID = "mengine.leanplum.app_id";
    public static final String METADATA_DEV_KEY = "mengine.leanplum.dev_key";
    public static final String METADATA_PROD_KEY = "mengine.leanplum.prod_key";
    public static final String METADATA_ENVIRONMENT = "mengine.leanplum.environment";

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        String AppId = this.getMetaDataString(METADATA_APP_ID);
        String DevKey = this.getMetaDataString(METADATA_DEV_KEY);
        String ProdKey = this.getMetaDataString(METADATA_PROD_KEY);
        String Environment = this.getMetaDataString(METADATA_ENVIRONMENT);

        this.logInfo("%s: %s"
            , METADATA_APP_ID
            , AppId
        );

        if (BuildConfig.DEBUG) {
            this.logInfo("%s: %s"
                , METADATA_DEV_KEY
                , DevKey
            );
        } else {
            this.logInfo("%s: %s"
                , METADATA_PROD_KEY
                , ProdKey
            );
        }

        this.logInfo("%s: %s"
            , METADATA_ENVIRONMENT
            , Environment
        );

        String sessionId = application.getSessionId();
        Leanplum.setUserId(sessionId);

        Leanplum.setApplicationContext(application);
        Parser.parseVariables(application);

        LeanplumActivityHelper.enableLifecycleCallbacks(application);

        if (BuildConfig.DEBUG) {
            Leanplum.setLogLevel(Log.Level.DEBUG);
            Leanplum.setAppIdForDevelopmentMode(AppId, DevKey);
        } else {
            Leanplum.setAppIdForProductionMode(AppId, ProdKey);
        }

        Leanplum.start(application);
    }

    @Override
    void onMengineSessionId(MengineApplication application, String sessionId) {
        Leanplum.setUserId(sessionId);
    }
}
