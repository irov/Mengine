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

public class MengineLeanplumPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "MengineLeanplum";

    public static final String PLUGIN_METADATA_APP_ID = "mengine.leanplum.app_id";
    public static final String PLUGIN_METADATA_DEV_KEY = "mengine.leanplum.dev_key";
    public static final String PLUGIN_METADATA_PROD_KEY = "mengine.leanplum.prod_key";
    public static final String PLUGIN_METADATA_ENVIRONMENT = "mengine.leanplum.environment";

    @Override
    public void onEvent(MengineApplication application, MengineEvent event, Object ... args) {
        if (event == MengineEvent.EVENT_SESSION_ID) {
            String sessionId = (String)args[0];

            Leanplum.setUserId(sessionId);
        }
    }

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        String AppId = this.getMetaDataString(PLUGIN_METADATA_APP_ID);
        String DevKey = this.getMetaDataString(PLUGIN_METADATA_DEV_KEY);
        String ProdKey = this.getMetaDataString(PLUGIN_METADATA_PROD_KEY);
        String Environment = this.getMetaDataString(PLUGIN_METADATA_ENVIRONMENT);

        this.logInfo("%s: %s"
            , PLUGIN_METADATA_APP_ID
            , AppId
        );

        if (BuildConfig.DEBUG) {
            this.logInfo("%s: %s"
                , PLUGIN_METADATA_DEV_KEY
                , DevKey
            );
        } else {
            this.logInfo("%s: %s"
                , PLUGIN_METADATA_PROD_KEY
                , ProdKey
            );
        }

        this.logInfo("%s: %s"
            , PLUGIN_METADATA_ENVIRONMENT
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
}
