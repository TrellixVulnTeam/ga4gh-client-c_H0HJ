#include <curl/curl.h>

#include "_protocol.h"

typedef struct {
    char *base_url;
    CURL *curl;
    struct curl_slist *headers;
    char *response;
    size_t response_length;
    size_t max_response_length;
} gaclient_t;

int gaclient_global_init(void);
int gaclient_global_free(void);

int gaclient_init(gaclient_t *self, const char *base_url);
int gaclient_free(gaclient_t *self);
int gaclient_search_variant_sets(gaclient_t *self);

#define GAERR_GENERIC -1
#define GAERR_NO_MEMORY -2
