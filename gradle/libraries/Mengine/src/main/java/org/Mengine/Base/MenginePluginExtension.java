package org.Mengine.Base;

public interface MenginePluginExtension {
    boolean onInitialize(MengineActivity activity, MenginePlugin plugin);
    void onFinalize(MengineActivity activity, MenginePlugin plugin);
    void onRun(MengineActivity activity, MenginePlugin plugin);
}
