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
    public static final String PLUGIN_NAME = "Leanplum";

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
        String AppId = application.getMetaDataString(PLUGIN_METADATA_APP_ID);
        String DevKey = application.getMetaDataString(PLUGIN_METADATA_DEV_KEY);
        String ProdKey = application.getMetaDataString(PLUGIN_METADATA_PROD_KEY);
        String Environment = application.getMetaDataString(PLUGIN_METADATA_ENVIRONMENT);

        if (AppId == null) {
            this.invalidInitialize("invalid setup meta data [%s]", PLUGIN_METADATA_APP_ID);

            return;
        }

        this.logInfo("%s: %s"
            , PLUGIN_METADATA_APP_ID
            , AppId
        );

        if (BuildConfig.DEBUG) {
            if (DevKey == null) {
                this.invalidInitialize("invalid setup meta data [%s]", PLUGIN_METADATA_DEV_KEY);

                return;
            }

            this.logInfo("%s: %s"
                , PLUGIN_METADATA_DEV_KEY
                , DevKey
            );
        } else {
            if (ProdKey == null) {
                this.invalidInitialize("invalid setup meta data [%s]", PLUGIN_METADATA_PROD_KEY);

                return;
            }

            this.logInfo("%s: %s"
                , PLUGIN_METADATA_PROD_KEY
                , ProdKey
            );
        }

        if (AppId == null) {
            this.invalidInitialize("invalid setup meta data [%s]", PLUGIN_METADATA_ENVIRONMENT);

            return;
        }

        this.logInfo("%s: %s"
            , PLUGIN_METADATA_ENVIRONMENT
            , Environment
        );

        Leanplum.setApplicationContext(application);
        Parser.parseVariables(application);

        LeanplumActivityHelper.enableLifecycleCallbacks(application);

        if (BuildConfig.DEBUG) {
            Leanplum.setLogLevel(Log.Level.DEBUG);
            Leanplum.setAppIdForDevelopmentMode("Your App ID", "Your Development Key");
        } else {
            Leanplum.setAppIdForProductionMode("Your App ID", "Your Production Key");
        }

        Leanplum.start(application);
    }
}
