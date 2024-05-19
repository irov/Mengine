package org.Mengine.Base;

import android.util.Base64;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.Map;

public class MengineNetwork {
    public static final String TAG = "MengineNetwork";

    @Nullable
    public static MengineHttpResponseParam httpRequestPostMessage(MengineHttpRequestParam request, Map<String, String> properties) {
        try {
            URL url = new URL(request.HTTP_URL);

            HttpURLConnection connection = (HttpURLConnection)url.openConnection();
            connection.setRequestMethod("POST");

            connection.setDoOutput(true);

            MengineNetwork.setTimeout(connection, request.HTTP_TIMEOUT);
            MengineNetwork.setHeaders(connection, request.HTTP_HEADERS);
            MengineNetwork.setMultipartFormData(connection, properties);

            connection.connect();

            MengineHttpResponseParam response = MengineNetwork.makeResponse(connection);

            connection.disconnect();

            return response;
        } catch (Exception e) {
            MengineLog.logError(TAG, "invalid http request post message: %s"
                , e.getMessage()
            );

            return null;
        }
    }

    @Nullable
    public static MengineHttpResponseParam httpRequestHeaderData(MengineHttpRequestParam request, byte[] data) {
        try {
            URL url = new URL(request.HTTP_URL);

            HttpURLConnection connection = (HttpURLConnection)url.openConnection();
            connection.setRequestMethod("POST");

            connection.setDoOutput(true);

            MengineNetwork.setTimeout(connection, request.HTTP_TIMEOUT);
            MengineNetwork.setHeaders(connection, request.HTTP_HEADERS);
            MengineNetwork.setData(connection, data);

            connection.connect();

            MengineHttpResponseParam response = MengineNetwork.makeResponse(connection);

            connection.disconnect();

            return response;
        } catch (Exception e) {
            MengineLog.logError(TAG, "invalid http request header data: %s"
                , e.getMessage()
            );

            return null;
        }
    }

    @Nullable
    public static MengineHttpResponseParam httpRequestGetMessage(MengineHttpRequestParam request) {
        try {
            URL url = new URL(request.HTTP_URL);

            HttpURLConnection connection = (HttpURLConnection)url.openConnection();
            connection.setRequestMethod("GET");

            MengineNetwork.setTimeout(connection, request.HTTP_TIMEOUT);
            MengineNetwork.setHeaders(connection, request.HTTP_HEADERS);

            connection.connect();

            MengineHttpResponseParam response = MengineNetwork.makeResponse(connection);

            connection.disconnect();

            return response;
        } catch (Exception e) {
            MengineLog.logError(TAG, "invalid http request get message: %s"
                , e.getMessage()
            );

            return null;
        }
    }

    @Nullable
    public static MengineHttpResponseParam httpRequestDeleteMessage(MengineHttpRequestParam request) {
        try {
            URL url = new URL(request.HTTP_URL);

            HttpURLConnection connection = (HttpURLConnection)url.openConnection();
            connection.setRequestMethod("DELETE");

            MengineNetwork.setTimeout(connection, request.HTTP_TIMEOUT);
            MengineNetwork.setHeaders(connection, request.HTTP_HEADERS);

            connection.connect();

            MengineHttpResponseParam response = MengineNetwork.makeResponse(connection);

            connection.disconnect();

            return response;
        } catch (Exception e) {
            MengineLog.logError(TAG, "invalid http request delete message: %s"
                , e.getMessage()
            );

            return null;
        }
    }

    @Nullable
    public static MengineHttpResponseParam httpRequestGetAsset(MengineHttpRequestParam request, String login, String password) {
        try {
            URL url = new URL(request.HTTP_URL);

            HttpURLConnection connection = (HttpURLConnection)url.openConnection();
            connection.setRequestMethod("GET");

            MengineNetwork.setTimeout(connection, request.HTTP_TIMEOUT);
            MengineNetwork.setBasicAuthorization(connection, login, password);
            MengineNetwork.setHeaders(connection, request.HTTP_HEADERS);

            connection.connect();

            MengineHttpResponseParam response = MengineNetwork.makeResponse(connection);

            connection.disconnect();

            return response;
        } catch (Exception e) {
            MengineLog.logError(TAG, "invalid http request get asset: %s"
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
        String userCredentials = login + ":" + password;

        String basicAuth = "Basic " + Base64.encodeToString(userCredentials.getBytes(), Base64.DEFAULT);
        connection.setRequestProperty("Authorization", basicAuth);
    }

    protected static void setData(@NonNull HttpURLConnection connection, @NonNull byte[] data) {
        try {
            OutputStream output = connection.getOutputStream();

            output.write(data);
            output.flush();
        } catch (Exception e) {
            MengineLog.logError(TAG, "invalid http request set data: %s"
                , e.getMessage()
            );
        }
    }

    protected static void setHeaders(@NonNull HttpURLConnection connection, @NonNull List<String> headers) {
        if (headers.isEmpty() == true) {
            return;
        }

        for (String header : headers) {
            String[] parts = header.split(":");

            if (parts.length != 2) {
                continue;
            }

            connection.setRequestProperty(parts[0], parts[1]);
        }
    }

    protected static void setMultipartFormData(@NonNull HttpURLConnection connection, @NonNull Map<String, String> properties) {
        if (properties.isEmpty() == true) {
            return;
        }

        String boundary = MengineUtils.getSecureRandomHexString(32);

        connection.setRequestProperty("Content-Type", "multipart/form-data; boundary=" + boundary);

        try {
            OutputStream output = connection.getOutputStream();

            Writer writer = new OutputStreamWriter(output, StandardCharsets.UTF_8);

            // Append parameters
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

            // End of multipart/form-data.
            writer.append("--").append(boundary).append("--").append("\r\n");
            writer.flush();
        } catch (Exception e) {
            MengineLog.logError(TAG, "invalid http request set multipart form data: %s"
                , e.getMessage()
            );
        }
    }

    protected static void setResponseCode(@NonNull HttpURLConnection connection, @NonNull MengineHttpResponseParam response) {
        try {
            int responseCode = connection.getResponseCode();

            response.HTTP_RESPONSE_CODE = responseCode;
        } catch (Exception e) {
            MengineLog.logError(TAG, "invalid http request set response code: %s"
                , e.getMessage()
            );
        }
    }

    protected static MengineHttpResponseParam makeResponse(@NonNull HttpURLConnection connection) {
        MengineHttpResponseParam response = new MengineHttpResponseParam();

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

    protected static void getResponseContentData(@NonNull HttpURLConnection connection, @NonNull MengineHttpResponseParam response) {
        try {
            InputStream is = connection.getInputStream();

            int length = connection.getContentLength();

            if (length != 0) {
                response.HTTP_CONTENT_DATA = new byte[length];
                response.HTTP_CONTENT_LENGTH = is.read(response.HTTP_CONTENT_DATA, 0, length);
            }

            is.close();
        } catch (Exception e) {
            MengineLog.logError(TAG, "invalid http request get response content data: %s"
                , e.getMessage()
            );
        }
    }

    protected static void getResponseErrorMessage(@NonNull HttpURLConnection connection, @NonNull MengineHttpResponseParam response) {
        try {
            InputStream is = connection.getErrorStream();

            String HTTP_ERROR_MESSAGE = MengineUtils.inputStreamToString(is);

            if (HTTP_ERROR_MESSAGE.isEmpty() == false) {
                response.HTTP_ERROR_MESSAGE = HTTP_ERROR_MESSAGE;
            }

            is.close();
        } catch (Exception e) {
            MengineLog.logError(TAG, "invalid http request get response error message: %s"
                , e.getMessage()
            );
        }
    }
}