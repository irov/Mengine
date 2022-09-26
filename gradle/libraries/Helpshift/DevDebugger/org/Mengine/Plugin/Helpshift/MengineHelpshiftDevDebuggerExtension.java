package org.Mengine.Plugin.Helpshifter;

import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginExtension;
import org.Mengine.Base.MengineActivity;

import org.Mengine.Plugin.Helpshift.MengineHelpshiftPlugin;

public class MengineHelpshiftDevDebuggerExtension implements MenginePluginExtension<MengineHelpshiftPlugin> {
    @Override
    public boolean initialize(MengineActivity activity, MengineHelpshiftPlugin plugin) {

        return true;
    }

    @Override
    public void finalize(MengineActivity activity, MengineHelpshiftPlugin plugin) {

    }

    @Override
    public void run(MengineActivity activity, MengineHelpshiftPlugin plugin) {

    }
}
