package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public final class MenginePluginExtensionManager {
    private static final MengineTag TAG = MengineTag.of("MNGExtMgr");

    private static final Map<String, List<Class<? extends MenginePluginExtensionInterface>>> m_extensions = new HashMap<>();
    private static final Map<String, List<MenginePluginExtensionInterface>> m_instances = new HashMap<>();

    public static void registerExtension(@NonNull String serviceName, @NonNull Class<? extends MenginePluginExtensionInterface> clazz) {
        m_extensions.computeIfAbsent(serviceName, k -> new ArrayList<>()).add(clazz);

        MengineLog.logInfo(TAG, "register extension %s for service %s", clazz.getName(), serviceName);
    }

    public static void loadExtensions(@NonNull String[] classNames) {
        for (String className : classNames) {
            try {
                Class.forName(className);
            } catch (ClassNotFoundException e) {
                MengineLog.logError(TAG, "extension class not found: %s", className);
            }
        }
    }

    public static void createExtensions(@NonNull String serviceName, @NonNull MengineApplication application) {
        List<Class<? extends MenginePluginExtensionInterface>> classes = m_extensions.get(serviceName);

        if (classes == null) {
            return;
        }

        List<MenginePluginExtensionInterface> instances = m_instances.computeIfAbsent(serviceName, k -> new ArrayList<>());

        for (Class<? extends MenginePluginExtensionInterface> clazz : classes) {
            try {
                MenginePluginExtensionInterface extension = clazz.newInstance();
                extension.onExtensionInitialize(application);

                instances.add(extension);

                MengineLog.logInfo(TAG, "created extension %s for service %s", clazz.getName(), serviceName);
            } catch (Exception e) {
                MengineLog.logError(TAG, "failed to create extension %s for service %s: %s", clazz.getName(), serviceName, e.getMessage());
            }
        }
    }

    public static void finalizeExtensions(@NonNull String serviceName, @NonNull MengineApplication application) {
        List<MenginePluginExtensionInterface> instances = m_instances.get(serviceName);

        if (instances == null) {
            return;
        }

        for (MenginePluginExtensionInterface extension : instances) {
            extension.onExtensionFinalize(application);
        }

        instances.clear();
    }
}
