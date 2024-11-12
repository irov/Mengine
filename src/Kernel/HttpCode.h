#pragma once

#include "Config/Typedef.h"

#define MENGINE_HTTP_REQUEST_TIMEOUT_INFINITY (-1)

namespace Mengine
{
    enum EHttpCode : uint32_t
    {
        HTTP_Unknown = 0,

        // Informational
        HTTP_Continue = 100,
        HTTP_SwitchingProtocols = 101,
        HTTP_Processing = 102,
        HTTP_EarlyHints = 103,

        // Successful
        HTTP_OK = 200,
        HTTP_Created = 201,
        HTTP_Accepted = 202,
        HTTP_NonAuthoritativeInformation = 203,
        HTTP_NoContent = 204,
        HTTP_ResetContent = 205,
        HTTP_PartialContent = 206,
        HTTP_MultiStatus = 207,
        HTTP_AlreadyReported = 208,
        HTTP_IMUsed = 226,

        // Redirection
        HTTP_MultipleChoices = 300,
        HTTP_MovedPermanently = 301,
        HTTP_Found = 302,
        HTTP_SeeOther = 303,
        HTTP_NotModified = 304,
        HTTP_UseProxy = 305,
        HTTP_TemporaryRedirect = 307,
        HTTP_PermanentRedirect = 308,

        // Client Error
        HTTP_BadRequest = 400,
        HTTP_Unauthorized = 401,
        HTTP_PaymentRequired = 402,
        HTTP_Forbidden = 403,
        HTTP_NotFound = 404,
        HTTP_MethodNotAllowed = 405,
        HTTP_NotAcceptable = 406,
        HTTP_ProxyAuthenticationRequired = 407,
        HTTP_RequestTimeout = 408,
        HTTP_Conflict = 409,
        HTTP_Gone = 410,
        HTTP_LengthRequired = 411,
        HTTP_PreconditionFailed = 412,
        HTTP_ContentTooLarge = 413,
        HTTP_PayloadTooLarge = 413,
        HTTP_URITooLong = 414,
        HTTP_UnsupportedMediaType = 415,
        HTTP_RangeNotSatisfiable = 416,
        HTTP_ExpectationFailed = 417,
        HTTP_ImATeapot = 418,
        HTTP_MisdirectedRequest = 421,
        HTTP_UnprocessableContent = 422,
        HTTP_UnprocessableEntity = 422,
        HTTP_Locked = 423,
        HTTP_FailedDependency = 424,
        HTTP_TooEarly = 425,
        HTTP_UpgradeRequired = 426,
        HTTP_PreconditionRequired = 428,
        HTTP_TooManyRequests = 429,
        HTTP_RequestHeaderFieldsTooLarge = 431,
        HTTP_UnavailableForLegalReasons = 451,

        // Server Error
        HTTP_InternalServerError = 500,
        HTTP_NotImplemented = 501,
        HTTP_BadGateway = 502,
        HTTP_ServiceUnavailable = 503,
        HTTP_GatewayTimeout = 504,
        HTTP_HTTPVersionNotSupported = 505,
        HTTP_VariantAlsoNegotiates = 506,
        HTTP_InsufficientStorage = 507,
        HTTP_LoopDetected = 508,
        HTTP_NotExtended = 510,
        HTTP_NetworkAuthenticationRequired = 511,
    };
}

#define HTTP_CODE_IS_INFORMATIONAL( code ) (code >= 100 && code < 200)
#define HTTP_CODE_IS_SUCCESSFUL( code ) (code >= 200 && code < 300)
#define HTTP_CODE_IS_REDIRECTION( code ) (code >= 300 && code < 400)
#define HTTP_CODE_IS_CLIENTERROR( code ) (code >= 400 && code < 500)
#define HTTP_CODE_IS_SERVERERROR( code ) (code >= 500 && code < 600)
#define HTTP_CODE_IS_ERROR( code ) (code >= 400 && code < 600)