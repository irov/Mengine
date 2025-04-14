package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.List;

public class MengineFragment<L extends MengineServiceInterface> implements MengineFragmentInterface {
    protected final Class<L> m_listenerClass;

    protected final List<L> m_listeners = new ArrayList<>();

    MengineFragment(@NonNull Class<L> listenerClass) {
        m_listenerClass = listenerClass;
    }

    public List<L> getListeners() {
        return m_listeners;
    }

    @SuppressWarnings("unchecked")
    @Override
    public void explainServiceListeners(@NonNull MengineServiceInterface service) {
        if (m_listenerClass.isInstance(service)) {
            L listener = (L) service;

            m_listeners.add(listener);
        }
    }

    protected void propagate(MenginePropagateV0<L> propagate) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (!listener.onAvailable(MengineApplication.INSTANCE)) {
                continue;
            }

            propagate.call(listener, MengineApplication.INSTANCE);
        }
    }

    protected <A1> void propagate(MenginePropagateV1<L, A1> propagate, A1 a1) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (!listener.onAvailable(MengineApplication.INSTANCE)) {
                continue;
            }

            propagate.call(listener, MengineApplication.INSTANCE, a1);
        }
    }

    protected <A1, A2> void propagate(MenginePropagateV2<L, A1, A2> propagate, A1 a1, A2 a2) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (!listener.onAvailable(MengineApplication.INSTANCE)) {
                continue;
            }

            propagate.call(listener, MengineApplication.INSTANCE, a1, a2);
        }
    }

    protected <A1, A2, A3> void propagate(MenginePropagateV3<L, A1, A2, A3> propagate, A1 a1, A2 a2, A3 a3) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (!listener.onAvailable(MengineApplication.INSTANCE)) {
                continue;
            }

            propagate.call(listener, MengineApplication.INSTANCE, a1, a2, a3);
        }
    }

    protected <A1, A2, A3, A4> void propagate(MenginePropagateV4<L, A1, A2, A3, A4> propagate, A1 a1, A2 a2, A3 a3, A4 a4) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (!listener.onAvailable(MengineApplication.INSTANCE)) {
                continue;
            }

            propagate.call(listener, MengineApplication.INSTANCE, a1, a2, a3, a4);
        }
    }

    protected <A1, A2, A3, A4, A5> void propagate(MenginePropagateV5<L, A1, A2, A3, A4, A5> propagate, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (!listener.onAvailable(MengineApplication.INSTANCE)) {
                continue;
            }

            propagate.call(listener, MengineApplication.INSTANCE, a1, a2, a3, a4, a5);
        }
    }

    protected boolean propagateB(MenginePropagateB0<L> propagate) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (!listener.onAvailable(MengineApplication.INSTANCE)) {
                continue;
            }

            if (propagate.call(listener, MengineApplication.INSTANCE) == true) {;
                return true;
            }
        }

        return false;
    }

    protected <A1> boolean propagateB(MenginePropagateB1<L, A1> propagate, A1 a1) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (!listener.onAvailable(MengineApplication.INSTANCE)) {
                continue;
            }

            if (propagate.call(listener, MengineApplication.INSTANCE, a1) == true) {
                return true;
            }
        }

        return false;
    }

    protected <A1, A2> boolean propagateB(MenginePropagateB2<L, A1, A2> propagate, A1 a1, A2 a2) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (!listener.onAvailable(MengineApplication.INSTANCE)) {
                continue;
            }

            if (propagate.call(listener, MengineApplication.INSTANCE, a1, a2) == true) {
                return true;
            }
        }

        return false;
    }

    protected <A1, A2, A3> boolean propagateB(MenginePropagateB3<L, A1, A2, A3> propagate, A1 a1, A2 a2, A3 a3) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (!listener.onAvailable(MengineApplication.INSTANCE)) {
                continue;
            }

            if (propagate.call(listener, MengineApplication.INSTANCE, a1, a2, a3) == true) {
                return true;
            }
        }

        return false;
    }

    protected <A1, A2, A3, A4> boolean propagateB(MenginePropagateB4<L, A1, A2, A3, A4> propagate, A1 a1, A2 a2, A3 a3, A4 a4) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (!listener.onAvailable(MengineApplication.INSTANCE)) {
                continue;
            }

            if (propagate.call(listener, MengineApplication.INSTANCE, a1, a2, a3, a4) == true) {
                return true;
            }
        }

        return false;
    }

    protected <A1, A2, A3, A4, A5> boolean propagateB(MenginePropagateB5<L, A1, A2, A3, A4, A5> propagate, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (!listener.onAvailable(MengineApplication.INSTANCE)) {
                continue;
            }

            if (propagate.call(listener, MengineApplication.INSTANCE, a1, a2, a3, a4, a5) == true) {
                return true;
            }
        }

        return false;
    }
}