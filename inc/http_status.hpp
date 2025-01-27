#ifndef HTTP_STATUS_HPP
#define HTTP_STATUS_HPP

//////////////////////////////////
//
//	       INCLUDES
//
/////////////////////////////////

#include <stddef.h>

//////////////////////////////////
//
//	       ENUM
//
/////////////////////////////////

enum http_status_code_t
{
	HTTP_STATUS_CONTINUE = 100,
	HTTP_STATUS_SWITCHING_PROT,
	HTTP_STATUS_OK = 200,
	HTTP_STATUS_CREATED,
	HTTP_STATUS_ACCEPTED,
	HTTP_STATUS_NON_AUTH_INF,
	HTTP_STATUS_NO_CONTENT,
	HTTP_STATUS_RESET_CONTENT,
	HTTP_STATUS_PARTIAL_CONTENT,
	HTTP_STATUS_MULTIPLE_CHOICES = 300,
	HTTP_STATUS_MOVED_PERMANENTLY,
	HTTP_STATUS_FOUND,
	HTTP_STATUS_SEE_OTHER,
	HTTP_STATUS_NOT_MODIFIED,
	HTTP_STATUS_USE_PROXY,
	HTTP_STATUS_TMP_REDIRECT,
	HTTP_STATUS_BAD_REQUEST = 400,
	HTTP_STATUS_UNAUTHORIZED,
	HTTP_STATUS_PAYMENT_REQUIRED,
	HTTP_STATUS_FORBIDDEN,
	HTTP_STATUS_NOT_FOUND,
	HTTP_STATUS_METHOD_NOT_ALLOWED,
	HTTP_STATUS_NOT_ACCEPTABLE,
	HTTP_STATUS_PROXY_AUTH_REQUIRED,
	HTTP_STATUS_REQUEST_TIMEOUT,
	HTTP_STATUS_CONFLICT,
	HTTP_STATUS_GONE,
	HTTP_STATUS_LENGTH_REQUIRED,
	HTTP_STATUS_PRECONDITION_FAILED,
	HTTP_STATUS_PAYLOAD_TOO_LARGE,
	HTTP_STATUS_URI_TOO_LONG,
	HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE,
	HTTP_STATUS_RANGE_NOT_SATISFIABLE,
	HTTP_STATUS_EXPECTATION_FAILED,
	HTTP_STATUS_UPGRADE_REQUIRED,
	HTTP_STATUS_INTERNAL_SERVER_ERROR = 500,
	HTTP_STATUS_NOT_IMPLEMENTED,
	HTTP_STATUS_BAD_GATEWAY,
	HTTP_STATUS_SERVICE_UNAVAILABLE,
	HTTP_STATUS_GATEWAY_TIMEOUT,
	HTTP_STATUS_VERSION_NOT_SUPPORTED
};

//////////////////////////////////
//
//	       TYPEDEF
//
/////////////////////////////////

typedef struct	http_status_t http_status_t;

//////////////////////////////////
//
//	       STRUCT
//
/////////////////////////////////

struct http_status_t
{
	const char			*str;
	http_status_code_t	code;
};

//////////////////////////////////
//
//	       FUNC
//
/////////////////////////////////

http_status_t	*get_http_status_table(size_t *size);
const char		*get_http_status_msg(http_status_code_t code);

#endif
