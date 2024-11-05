package org.Mengine.Base;

import java.util.List;

public class MengineHttpRequestParam {
    public MengineHttpRequestParam(String url, String proxy, List<String> headers, String cookies, int timeout) {
        this.HTTP_URL = url;
        this.HTTP_PROXY = proxy;
        this.HTTP_HEADERS = headers;
        this.HTTP_COOKIES = cookies;
        this.HTTP_TIMEOUT = timeout;
    }

    public final String HTTP_URL;
    public final String HTTP_PROXY;
    public final List<String> HTTP_HEADERS;
    public final String HTTP_COOKIES;
    public final int HTTP_TIMEOUT;
}