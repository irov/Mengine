package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.lang.reflect.Constructor;
import java.util.HashMap;
import java.util.Map;

public final class MengineFactoryManager {
    private static final class Entry {
        final MengineFactory factory;
        final Class<?>[] types;

        Entry(MengineFactory factory, Class<?>... types) {
            this.factory = factory;
            this.types = types;
        }
    }

    private static final Map<String, Entry> m_factories = new HashMap<>();

    public static void registerFactory(@NonNull String id, @NonNull MengineFactory factory, Class<?> ... types) {
        m_factories.put(id, new Entry(factory, types));
    }

    public static void registerClazz(@NonNull String id, @NonNull Class<? extends MengineFactorable> clazz, Class<?> ... types) {
        try {
            Constructor<? extends MengineFactorable> ctor =
                    clazz.getDeclaredConstructor(types);
            ctor.setAccessible(true);

            MengineFactory factory = args -> {
                try {
                    return ctor.newInstance(args);
                } catch (ReflectiveOperationException ex) {
                    throw new RuntimeException(String.format("factory %s failed to create instance of %s exception: %s", id, clazz.getName(), ex));
                }
            };

            MengineFactoryManager.registerFactory(id, factory, types);
        } catch (NoSuchMethodException ex) {
            throw new IllegalArgumentException(String.format("factory %s class %s does not have a constructor with the specified parameter types exception: %s", id, clazz.getName(), ex));
        }
    }

    public static MengineFactorable build(@NonNull String id, Object... args) throws IllegalArgumentException {
        Entry entry = m_factories.get(id);

        if (entry == null) {
            throw new IllegalArgumentException(String.format("factory %s not found", id));
        }

        Class<?>[] expected = entry.types;

        if (expected.length != args.length) {
            throw new IllegalArgumentException(String.format("factory %s expected %d arguments, but got %d", id, expected.length, args.length));
        }

        for (int i = 0; i != expected.length; ++i) {
            Class<?> expect = expected[i];
            Object arg = args[i];

            if (arg != null && expect.isInstance(arg) == false) {
                throw new IllegalArgumentException(String.format("factory %s argument %d is not of type %s: %s", id, i, expect.getName(), arg));
            }
        }

        MengineFactory factory = entry.factory;

        MengineFactorable factorable = factory.create(args);

        return factorable;
    }
}