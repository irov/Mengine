package org.Mengine.Plugin.Helpshift;

import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginExtension;
import org.Mengine.Base.MengineActivity;

import org.Mengine.Plugin.DevDebugger.MengineDevDebuggerPlugin;
import org.Mengine.Plugin.Helpshift.MengineHelpshiftPlugin;

public class MengineHelpshiftDevDebuggerExtension implements MenginePluginExtension {
    @Override
    public boolean onInitialize(MengineActivity activity, MenginePlugin plugin) {

        return true;
    }

    @Override
    public void onFinalize(MengineActivity activity, MenginePlugin plugin) {

    }

    @Override
    public void onRun(MengineActivity activity, MenginePlugin plugin) {
        MengineDevDebuggerPlugin devDebugger = activity.findPlugin();

    }
}
