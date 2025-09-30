package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

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

    private static String buildJsonArgs(List<?> args) {
        StringBuilder sb = new StringBuilder();

        sb.append("[");

        for (int i = 0; i != args.size(); ++i) {
            Object arg = args.get(i);

            if (i != 0) {
                sb.append(",");
            }

            if (arg == null) {
                sb.append("null");
            } else if (arg instanceof String) {
                sb.append("\"");
                sb.append(arg);
                sb.append("\"");
            } else if (arg instanceof Number) {
                sb.append(arg);
            } else if (arg instanceof Boolean) {
                sb.append(arg);
            } else {
                sb.append("\"");
                sb.append(arg);
                sb.append("\"");
            }
        }

        sb.append("]");

        return sb.toString();
    }

    private void logException(Object propagate, L listener, Throwable t, List<?> args) {
        MengineTag tag = listener.getServiceTag();

        MengineLog.logException(tag, t
            , Map.of(
            "propagate", propagate.getClass().getSimpleName(),
            "listener", listener.getServiceName(),
            "args", MengineFragment.buildJsonArgs(args)
            )
        );
    }

    protected void propagate(MenginePropagateV0<? super L> propagate) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (listener.onAvailable(MengineApplication.INSTANCE) == false) {
                continue;
            }

            try {
                propagate.call(listener, MengineApplication.INSTANCE);
            } catch (Throwable exception) {
                this.logException(propagate, listener, exception, List.of());
            }
        }
    }

    protected <A1> void propagate(MenginePropagateV1<? super L, A1> propagate, A1 a1) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (listener.onAvailable(MengineApplication.INSTANCE) == false) {
                continue;
            }

            try {
                propagate.call(listener, MengineApplication.INSTANCE, a1);
            } catch (Throwable exception) {
                this.logException(propagate, listener, exception, List.of(a1));
            }
        }
    }

    protected <A1, A2> void propagate(MenginePropagateV2<? super L, A1, A2> propagate, A1 a1, A2 a2) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (listener.onAvailable(MengineApplication.INSTANCE) == false) {
                continue;
            }

            try {
                propagate.call(listener, MengineApplication.INSTANCE, a1, a2);
            } catch (Throwable exception) {
                this.logException(propagate, listener, exception, List.of(a1, a2));
            }
        }
    }

    protected <A1, A2, A3> void propagate(MenginePropagateV3<? super L, A1, A2, A3> propagate, A1 a1, A2 a2, A3 a3) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (listener.onAvailable(MengineApplication.INSTANCE) == false) {
                continue;
            }

            try {
                propagate.call(listener, MengineApplication.INSTANCE, a1, a2, a3);
            } catch (Throwable exception) {
                this.logException(propagate, listener, exception, List.of(a1, a2, a3));
            }
        }
    }

    protected <A1, A2, A3, A4> void propagate(MenginePropagateV4<? super L, A1, A2, A3, A4> propagate, A1 a1, A2 a2, A3 a3, A4 a4) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (listener.onAvailable(MengineApplication.INSTANCE) == false) {
                continue;
            }

            try {
                propagate.call(listener, MengineApplication.INSTANCE, a1, a2, a3, a4);
            } catch (Throwable exception) {
                this.logException(propagate, listener, exception, List.of(a1, a2, a3, a4));
            }
        }
    }

    protected <A1, A2, A3, A4, A5> void propagate(MenginePropagateV5<? super L, A1, A2, A3, A4, A5> propagate, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (listener.onAvailable(MengineApplication.INSTANCE) == false) {
                continue;
            }

            try {
                propagate.call(listener, MengineApplication.INSTANCE, a1, a2, a3, a4, a5);
            } catch (Throwable exception) {
                this.logException(propagate, listener, exception, List.of(a1, a2, a3, a4, a5));
            }
        }
    }

    protected boolean propagateB(MenginePropagateB0<? super L> propagate) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (listener.onAvailable(MengineApplication.INSTANCE) == false) {
                continue;
            }

            try {
                if (propagate.call(listener, MengineApplication.INSTANCE) == true) {
                    return true;
                }
            } catch (Throwable exception) {
                this.logException(propagate, listener, exception, List.of());
            }
        }

        return false;
    }

    protected <A1> boolean propagateB(MenginePropagateB1<? super L, A1> propagate, A1 a1) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (listener.onAvailable(MengineApplication.INSTANCE) == false) {
                continue;
            }

            try {
                if (propagate.call(listener, MengineApplication.INSTANCE, a1) == true) {
                    return true;
                }
            } catch (Throwable exception) {
                this.logException(propagate, listener, exception, List.of(a1));
            }
        }

        return false;
    }

    protected <A1, A2> boolean propagateB(MenginePropagateB2<? super L, A1, A2> propagate, A1 a1, A2 a2) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (listener.onAvailable(MengineApplication.INSTANCE) == false) {
                continue;
            }

            try {
                if (propagate.call(listener, MengineApplication.INSTANCE, a1, a2) == true) {
                    return true;
                }
            } catch (Throwable exception) {
                this.logException(propagate, listener, exception, List.of(a1, a2));
            }
        }

        return false;
    }

    protected <A1, A2, A3> boolean propagateB(MenginePropagateB3<? super L, A1, A2, A3> propagate, A1 a1, A2 a2, A3 a3) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (listener.onAvailable(MengineApplication.INSTANCE) == false) {
                continue;
            }

            try {
                if (propagate.call(listener, MengineApplication.INSTANCE, a1, a2, a3) == true) {
                    return true;
                }
            } catch (Throwable exception) {
                this.logException(propagate, listener, exception, List.of(a1, a2, a3));
            }
        }

        return false;
    }

    protected <A1, A2, A3, A4> boolean propagateB(MenginePropagateB4<? super L, A1, A2, A3, A4> propagate, A1 a1, A2 a2, A3 a3, A4 a4) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (listener.onAvailable(MengineApplication.INSTANCE) == false) {
                continue;
            }

            try {
                if (propagate.call(listener, MengineApplication.INSTANCE, a1, a2, a3, a4) == true) {
                    return true;
                }
            } catch (Throwable exception) {
                this.logException(propagate, listener, exception, List.of(a1, a2, a3, a4));
            }
        }

        return false;
    }

    protected <A1, A2, A3, A4, A5> boolean propagateB(MenginePropagateB5<? super L, A1, A2, A3, A4, A5> propagate, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
        List<L> listeners = this.getListeners();

        for (L listener : listeners) {
            if (listener.onAvailable(MengineApplication.INSTANCE) == false) {
                continue;
            }

            try {
                if (propagate.call(listener, MengineApplication.INSTANCE, a1, a2, a3, a4, a5) == true) {
                    return true;
                }
            } catch (Throwable exception) {
                this.logException(propagate, listener, exception, List.of(a1, a2, a3, a4, a5));
            }
        }

        return false;
    }
}