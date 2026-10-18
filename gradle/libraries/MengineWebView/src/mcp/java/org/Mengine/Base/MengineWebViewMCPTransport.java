package org.Mengine.Base;

import android.os.Handler;
import android.os.Looper;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.RejectedExecutionException;

final class MengineWebViewMCPTransport {
    private static final int MAX_PAYLOAD_SIZE = 16 * 1024 * 1024;

    interface HandlerInterface {
        void onWebViewMCPTransportRequest(String method, JSONObject params, int id) throws JSONException;
        void onWebViewMCPTransportDisconnect();
    }

    private final Handler m_main;
    private final ExecutorService m_output;
    private final HandlerInterface m_handler;
    private volatile Socket m_socket;
    private volatile boolean m_stopped;

    MengineWebViewMCPTransport(HandlerInterface handler) {
        Looper mainLooper = Looper.getMainLooper();
        m_main = new Handler(mainLooper);
        m_output = Executors.newSingleThreadExecutor();
        m_handler = handler;
    }

    void start(String host, int port, String token) {
        new Thread(() -> {
            try (Socket socket = new Socket(host, port)) {
                m_socket = socket;

                if (m_stopped == true) {
                    return;
                }

                JSONArray capabilities = new JSONArray();
                String[] methods = {"script_call", "logs_read", "frame_capture", "app_stop"};

                for (String value : methods) {
                    capabilities.put(value);
                }

                JSONObject handshake = new JSONObject();
                handshake.put("method", "handshake");
                JSONObject params = new JSONObject();
                params.put("token", token);
                params.put("capabilities", capabilities);
                handshake.put("params", params);
                String handshakeJson = handshake.toString();
                byte[] handshakeBytes = handshakeJson.getBytes(StandardCharsets.UTF_8);
                this.send(1, 0, handshakeBytes);
                InputStream stream = socket.getInputStream();
                DataInputStream input = new DataInputStream(stream);

                for (;;) {
                    byte[] bytes = new byte[24];
                    input.readFully(bytes);
                    ByteBuffer header = ByteBuffer.wrap(bytes);
                    header.order(ByteOrder.LITTLE_ENDIAN);

                    int magic = header.getInt(0);

                    if (magic != 0x50434e4d) {
                        throw new IOException("invalid MNCP header");
                    }

                    short version = header.getShort(4);

                    if (version != 1) {
                        throw new IOException("invalid MNCP header");
                    }

                    int size = header.getInt(12);

                    if (size < 0 || size > MAX_PAYLOAD_SIZE) {
                        throw new IOException("invalid MNCP payload size");
                    }

                    byte[] payload = new byte[size];
                    input.readFully(payload);

                    if (bytes[6] != 1) {
                        continue;
                    }

                    int id = header.getInt(8);
                    String json = new String(payload, StandardCharsets.UTF_8);
                    JSONObject request = new JSONObject(json);
                    m_main.post(() -> {
                        if (m_stopped == true) {
                            return;
                        }

                        try {
                            String method = request.getString("method");
                            JSONObject arguments = request.optJSONObject("params");
                            m_handler.onWebViewMCPTransportRequest(method, arguments, id);
                        } catch (JSONException | RuntimeException error) {
                            String message = error.toString();
                            this.error(id, message);
                        }
                    });
                }
            } catch (IOException | JSONException error) {
                if (m_stopped == false) {
                    android.util.Log.e("MengineWebViewMCP", "MCP disconnected", error);
                    m_main.post(() -> {
                        if (m_stopped == true) {
                            return;
                        }

                        m_handler.onWebViewMCPTransportDisconnect();
                    });
                }

                m_output.shutdown();
            }
        }, "MengineWebViewMCPTransport").start();
    }

    void stop() {
        m_stopped = true;
        m_main.removeCallbacksAndMessages(null);
        Socket socket = m_socket;

        if (socket != null) {
            try {
                socket.close();
            } catch (IOException error) {
                android.util.Log.e("MengineWebViewMCP", "MCP close failed", error);
            }
        }

        m_output.shutdownNow();
    }

    void reply(int id, Object result) {
        JSONObject response = new JSONObject();
        try {
            Object value = result == null ? JSONObject.NULL : result;
            response.put("result", value);
        } catch (JSONException error) {
            this.error(id, "Cannot serialize MCP response");

            return;
        }

        this.sendJSON(id, response);
    }

    void error(int id, String message) {
        JSONObject value = new JSONObject();
        JSONObject details = new JSONObject();
        try {
            details.put("code", "execution_error");
            details.put("message", message);
            value.put("error", details);
        } catch (JSONException error) {
            throw new IllegalStateException(error);
        }

        this.sendJSON(id, value);
    }

    void binary(int id, byte[] bytes) {
        this.send(4, id, bytes);
    }

    void afterReplies(Runnable action) {
        Runnable callback = () -> {
            if (m_stopped == true) {
                return;
            }

            action.run();
        };

        try {
            m_output.execute(() -> {
                m_main.post(callback);
            });
        } catch (RejectedExecutionException error) {
            m_main.post(callback);
        }
    }

    private void sendJSON(int id, JSONObject value) {
        String json = value.toString();

        if (json == null) {
            this.error(id, "Cannot serialize MCP response");

            return;
        }

        byte[] bytes = json.getBytes(StandardCharsets.UTF_8);

        if (bytes.length > MAX_PAYLOAD_SIZE) {
            this.error(id, "MCP response exceeds the maximum payload size");

            return;
        }

        this.send(2, id, bytes);
    }

    private void send(int type, int id, byte[] payload) {
        int frameSize = 24 + payload.length;
        ByteBuffer frame = ByteBuffer.allocate(frameSize);
        frame.order(ByteOrder.LITTLE_ENDIAN);
        frame.putInt(0x50434e4d);
        short version = 1;
        frame.putShort(version);
        byte frameType = (byte)type;
        frame.put(frameType);
        byte flags = (byte)(type == 4 ? 2 : 3);
        frame.put(flags);
        frame.putInt(id);
        frame.putInt(payload.length);
        frame.putInt(0);
        frame.putInt(1);
        frame.put(payload);
        try {
            m_output.execute(() -> {
                try {
                    Socket socket = m_socket;

                    if (socket == null) {
                        return;
                    }

                    if (socket.isClosed() == true) {
                        return;
                    }

                    OutputStream output = socket.getOutputStream();
                    byte[] bytes = frame.array();
                    output.write(bytes);
                } catch (IOException error) {
                    android.util.Log.e("MengineWebViewMCP", "MCP send failed", error);
                }
            });
        } catch (RejectedExecutionException error) {
            //Empty
        }
    }
}
