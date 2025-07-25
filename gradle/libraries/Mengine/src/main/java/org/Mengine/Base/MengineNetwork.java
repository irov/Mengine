package org.Mengine.Base;

import android.util.Base64;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.UnknownHostException;
import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.Map;

public class MengineNetwork {
    public static final MengineTag TAG = MengineTag.of("MengineNetwork");

    public static boolean m_networkAvailable = true;
    public static boolean m_networkUnmetered = true;
    public static MengineNetworkTransport m_networkTransport = MengineNetworkTransport.NETWORKTRANSPORT_UNKNOWN;

    public static void setNetworkAvailable(boolean available) {
        m_networkAvailable = available;
    }

    public static boolean isNetworkAvailable() {
        return m_networkAvailable;
    }

    public static void setNetworkUnmetered(boolean unmetered) {
        m_networkUnmetered = unmetered;
    }

    public static boolean isNetworkUnmetered() {
        return m_networkUnmetered;
    }

    public static void setNetworkTransport(@NonNull MengineNetworkTransport transport) {
        m_networkTransport = transport;
    }

    public static MengineNetworkTransport getNetworkTransport() {
        return m_networkTransport;
    }

    protected static HttpURLConnection openConnection(@NonNull MengineParamHttpRequest request, String method, boolean output) throws IOException {
        URL httpUrl = new URL(request.HTTP_URL);

        HttpURLConnection connection = (HttpURLConnection)httpUrl.openConnection();

        connection.setRequestMethod(method);
        connection.setDoOutput(output);

        MengineApplication application = MengineApplication.INSTANCE;

        String installId = application.getInstallId();
        long installRND = application.getInstallRND();
        String applicationId = application.getApplicationId();
        int versionCode = application.getVersionCode();
        String versionName = application.getVersionName();

        connection.setRequestProperty("Mengine-Build-Type", BuildConfig.BUILD_TYPE);
        connection.setRequestProperty("Mengine-Platform", "Android");
        connection.setRequestProperty("Mengine-Install-Id", installId);
        connection.setRequestProperty("Mengine-Install-RND", String.valueOf(installRND));
        connection.setRequestProperty("Mengine-Application-Id", applicationId);
        connection.setRequestProperty("Mengine-Application-Code", String.valueOf(versionCode));
        connection.setRequestProperty("Mengine-Application-Version", versionName);

        String userId = application.getUserId();

        connection.setRequestProperty("Mengine-User-Id", userId);

        MengineNetwork.setTimeout(connection, request.HTTP_TIMEOUT);
        MengineNetwork.setHeaders(connection, request.HTTP_HEADERS);

        return connection;
    }

    protected static MengineParamHttpResponse processConnectionData(@NonNull HttpURLConnection connection) throws IOException {
        connection.connect();

        MengineParamHttpResponse response = MengineNetwork.makeResponseData(connection);

        connection.disconnect();

        return response;
    }

    @Nullable
    public static MengineParamHttpResponse httpRequestPing(@NonNull MengineParamHttpRequest request) {
        if (m_networkAvailable == false) {
            return null;
        }

        try {
            HttpURLConnection connection = MengineNetwork.openConnection(request, "HEAD", false);

            MengineParamHttpResponse response = MengineNetwork.processConnectionData(connection);

            return response;
        } catch (final Exception e) {
            MengineParamHttpResponse response = MengineNetwork.catchException(request, e);

            return response;
        }
    }

    @Nullable
    public static MengineParamHttpResponse httpRequestPostMessage(@NonNull MengineParamHttpRequest request, Map<String, String> properties) {
        if (m_networkAvailable == false) {
            return null;
        }

        try {
            HttpURLConnection connection = MengineNetwork.openConnection(request, "POST", true);

            MengineNetwork.setMultipartFormData(connection, properties);

            MengineParamHttpResponse response = MengineNetwork.processConnectionData(connection);

            return response;
        } catch (final Exception e) {
            MengineParamHttpResponse response = MengineNetwork.catchException(request, e);

            return response;
        }
    }

    @Nullable
    public static MengineParamHttpResponse httpRequestHeaderData(@NonNull MengineParamHttpRequest request, byte[] data) {
        if (m_networkAvailable == false) {
            return null;
        }

        try {
            HttpURLConnection connection = MengineNetwork.openConnection(request, "POST", true);

            MengineNetwork.setData(connection, data);

            MengineParamHttpResponse response = MengineNetwork.processConnectionData(connection);

            return response;
        } catch (final Exception e) {
            MengineParamHttpResponse response = MengineNetwork.catchException(request, e);

            return response;
        }
    }

    @Nullable
    public static MengineParamHttpResponse httpRequestGetMessage(@NonNull MengineParamHttpRequest request) {
        if (m_networkAvailable == false) {
            return null;
        }

        try {
            HttpURLConnection connection = MengineNetwork.openConnection(request, "GET", false);

            MengineParamHttpResponse response = MengineNetwork.processConnectionData(connection);

            return response;
        } catch (final Exception e) {
            MengineParamHttpResponse response = MengineNetwork.catchException(request, e);

            return response;
        }
    }

    @Nullable
    public static MengineParamHttpResponse httpRequestDeleteMessage(@NonNull MengineParamHttpRequest request) {
        if (m_networkAvailable == false) {
            return null;
        }

        try {
            HttpURLConnection connection = MengineNetwork.openConnection(request, "DELETE", false);

            MengineParamHttpResponse response = MengineNetwork.processConnectionData(connection);

            return response;
        } catch (final Exception e) {
            MengineParamHttpResponse response = MengineNetwork.catchException(request, e);

            return response;
        }
    }

    @Nullable
    public static MengineParamHttpResponse httpRequestGetAsset(@NonNull MengineParamHttpRequest request, String login, String password) {
        if (m_networkAvailable == false) {
            return null;
        }

        try {
            HttpURLConnection connection = MengineNetwork.openConnection(request, "GET", false);

            MengineNetwork.setBasicAuthorization(connection, login, password);

            MengineParamHttpResponse response = MengineNetwork.processConnectionData(connection);

            return response;
        } catch (final Exception e) {
            MengineParamHttpResponse response = MengineNetwork.catchException(request, e);

            return response;
        }
    }

    protected static MengineParamHttpResponse catchException(@NonNull MengineParamHttpRequest request, @NonNull Exception exception) {
        try {
            throw exception;
        } catch (final UnknownHostException e) {
            MengineParamHttpResponse response = new MengineParamHttpResponse();

            response.HTTP_RESPONSE_CODE = HttpURLConnection.HTTP_NOT_FOUND;
            response.HTTP_CONTENT_LENGTH = 0;
            response.HTTP_CONTENT_DATA = null;
            response.HTTP_ERROR_MESSAGE = e.getMessage();

            return response;
        } catch (final Exception e) {
            MengineLog.logMessage(TAG, "invalid http request url: %s exception: %s"
                , request.HTTP_URL
                , e.getMessage()
            );

            return null;
        }
    }

    protected static void setTimeout(@NonNull HttpURLConnection connection, int timeout) {
        if (timeout == -1) {
            return;
        }

        connection.setConnectTimeout(timeout);
    }

    protected static void setBasicAuthorization(@NonNull HttpURLConnection connection, @NonNull String login, @NonNull String password) {
        if (login.isEmpty() == true) {
            return;
        }

        String userCredentials = login + ":" + password;

        String basicAuth = "Basic " + Base64.encodeToString(userCredentials.getBytes(StandardCharsets.UTF_8), Base64.DEFAULT);
        connection.setRequestProperty("Authorization", basicAuth);
    }

    protected static void setData(@NonNull HttpURLConnection connection, @NonNull byte[] data) throws IOException {
        OutputStream output = connection.getOutputStream();

        output.write(data);
        output.flush();
    }

    protected static void setHeaders(@NonNull HttpURLConnection connection, @NonNull List<String> headers) {
        if (headers.isEmpty() == true) {
            return;
        }

        for (String header : headers) {
            List<String> parts = MengineUtils.splitToList(header, ":");

            if (parts.size() != 2) {
                continue;
            }

            String key = parts.get(0);
            String value = parts.get(1);

            connection.setRequestProperty(key, value);
        }
    }

    protected static void setMultipartFormData(@NonNull HttpURLConnection connection, @NonNull Map<String, String> properties) throws IOException {
        if (properties.isEmpty() == true) {
            return;
        }

        String boundary = MengineUtils.getSecureRandomHexString(32);

        connection.setRequestProperty("Content-Type", "multipart/form-data; boundary=" + boundary);

        OutputStream output = connection.getOutputStream();

        Writer writer = new OutputStreamWriter(output, StandardCharsets.UTF_8);

        for (Map.Entry<String, String> entry : properties.entrySet()) {
            String key = entry.getKey();
            String value = entry.getValue();

            writer.append("--").append(boundary).append("\r\n");
            writer.append("Content-Disposition: form-data; name=\"").append(key).append("\"").append("\r\n");
            writer.append("Content-Type: text/plain; charset=UTF-8").append("\r\n");
            writer.append("\r\n");
            writer.append(value).append("\r\n");
            writer.flush();
        }

        writer.append("--").append(boundary).append("--").append("\r\n");
        writer.flush();
    }

    protected static void setResponseCode(@NonNull HttpURLConnection connection, @NonNull MengineParamHttpResponse response) throws IOException {
        int responseCode = connection.getResponseCode();

        response.HTTP_RESPONSE_CODE = responseCode;
    }

    protected static MengineParamHttpResponse makeResponseData(@NonNull HttpURLConnection connection) throws IOException {
        MengineParamHttpResponse response = new MengineParamHttpResponse();

        MengineNetwork.setResponseCode(connection, response);

        switch (response.HTTP_RESPONSE_CODE) {
            case HttpURLConnection.HTTP_OK:
            case HttpURLConnection.HTTP_CREATED:
            case HttpURLConnection.HTTP_ACCEPTED:
            case HttpURLConnection.HTTP_NOT_AUTHORITATIVE:
            case HttpURLConnection.HTTP_NO_CONTENT:
            case HttpURLConnection.HTTP_RESET:
            case HttpURLConnection.HTTP_PARTIAL: {
                 MengineNetwork.getResponseContentData(connection, response);
            }break;
            default: {
                MengineNetwork.getResponseErrorMessage(connection, response);
            }break;
        }

        return response;
    }

    protected static void getResponseContentData(@NonNull HttpURLConnection connection, @NonNull MengineParamHttpResponse response) throws IOException {
        InputStream is = connection.getInputStream();

        int length = connection.getContentLength();

        if (length == 0) {
            response.HTTP_CONTENT_LENGTH = 0;
        } else if (length == -1) {
            byte [] data = MengineUtils.inputStreamToByteArray(is);

            response.HTTP_CONTENT_DATA = data;
            response.HTTP_CONTENT_LENGTH = data.length;
        } else {
            byte [] data = MengineUtils.inputStreamToByteArraySize(is, length);

            response.HTTP_CONTENT_DATA = data;
            response.HTTP_CONTENT_LENGTH = data.length;
        }

        is.close();
    }

    protected static void getResponseErrorMessage(@NonNull HttpURLConnection connection, @NonNull MengineParamHttpResponse response) throws IOException {
        InputStream is = connection.getErrorStream();

        String HTTP_ERROR_MESSAGE = MengineUtils.inputStreamToString(is);

        if (HTTP_ERROR_MESSAGE.isEmpty() == false) {
            response.HTTP_ERROR_MESSAGE = HTTP_ERROR_MESSAGE;
        }

        is.close();
    }
}