#include "./basic.h"

#include <stdio.h>

#define CRLF "\r\n"

int main()
{	
    char* http_res = "" 
    	"HTTP/2 200"CRLF
	"access-control-allow-credentials: true"CRLF
	"access-control-allow-headers: Origin, X-Requested-With, Content-Type, Accept"CRLF
	"access-control-allow-methods: GET, POST, PUT, PATCH, DELETE, OPTIONS, HEAD"CRLF
	"access-control-allow-origin: *"CRLF
	"age: 1"CRLF
	"cache-control: no-cache"CRLF
	"content-type: application/json"CRLF
	"date: Mon, 15 Jan 2024 17:56:22 GMT"CRLF
	"netlify-vary: query"CRLF
	"server: Netlify"CRLF
	"strict-transport-security: max-age=31536000"CRLF
	"x-nf-request-id: 01HM748V8STNF5ZM0CM13FZVBB";
    
    StringView sv = sv_from_cstr(http_res);

    StringView status_line = sv_chop_delim(&sv, '\n');
    StringView version = sv_chop_delim(&status_line, ' ');
    StringView status_code = sv_chop_delim(&status_line, ' ');
    printf("Version:%.*s\n", SV_Arg(version));
    printf("Code:%.*s\n", SV_Arg(status_code));
}
