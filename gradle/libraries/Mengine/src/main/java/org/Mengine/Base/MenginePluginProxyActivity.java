package org.Mengine.Base;

import android.app.Activity;
import android.os.Bundle;

public class MenginePluginProxyActivity extends Activity {
    public static final String TAG = "MenginePluginActivity";

    public MenginePlugin getPlugin(MengineApplication application) {
        return null;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        MengineApplication application = (MengineApplication)this.getApplication();

        MenginePlugin plugin = this.getPlugin(application);

        if (plugin instanceof MenginePluginProxyActivityListener listener) {
            try {
                listener.onProxyActivityCreate(this, savedInstanceState, new Runnable() {
                    @Override
                    public void run() {
                        try {
                            MengineUtils.nextChainActivity(MenginePluginProxyActivity.this);
                        } catch (MengineChainActivityNotFoundException e) {
                            MengineUtils.finishActivityWithAlertDialog(MenginePluginProxyActivity.this, "[ERROR] not found chain activity: %s"
                                , e.getClassName()
                            );
                        }
                    }
                });
            } catch (MenginePluginInvalidInitializeException e) {
                MengineUtils.finishActivityWithAlertDialog(this, "[ERROR] plugin: %s invalid initialize exception: %s"
                    , plugin.getPluginName()
                    , e.getMessage()
                );
            }
        } else {
            MengineUtils.finishActivityWithAlertDialog(this, "[ERROR] plugin: %s has not proxy activity listener"
                , plugin.getPluginName()
            );
        }
    }
}
