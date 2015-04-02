#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <jansson.h>

#include "ga4gh.h"

static size_t
gaclient_process_incoming_data(gaclient_t *self, void *buffer, size_t size)
{
    size_t ret = size;
    printf("CURL got %d bytes\n", (int) size);
    if (self->response_length + size > self->max_response_length) {
        ret = 0;
        /* TODO handle this properly by reallocing response */
        goto out;
    }
    memcpy(self->response + self->response_length, buffer, size);
    self->response_length += size;
out:
    return ret;
}

static size_t
_curl_write_data_callback(void *buffer, size_t size, size_t nmemb, void *userp)
{
    gaclient_t *self = (gaclient_t *) userp;
    return gaclient_process_incoming_data(self, buffer, size * nmemb);
}

int
gaclient_init(gaclient_t *self, const char *base_url)
{

    int ret = GAERR_GENERIC;
    size_t base_url_len;

    memset(self, 0, sizeof(self));
    base_url_len = strlen(base_url);
    self->base_url = malloc(base_url_len + 1);
    if (self->base_url == NULL) {
        ret = GAERR_NO_MEMORY;
        goto out;
    }
    strcpy(self->base_url, base_url);
    self->max_response_length = 8 * 2<<10; /* arbitrary */
    self->response_length = 0;
    self->response = malloc(self->max_response_length);
    if (self->response == NULL) {
        ret = GAERR_NO_MEMORY;
        goto out;
    }
    self->curl = curl_easy_init();
    if (self->curl == NULL) {
        /* TODO curl errors? */
        goto out;
    }
    /* curl_easy_setopt(self->curl, CURLOPT_VERBOSE, 1); */
    curl_easy_setopt(self->curl, CURLOPT_WRITEFUNCTION,
        _curl_write_data_callback);
    curl_easy_setopt(self->curl, CURLOPT_WRITEDATA, self);
    self->headers = curl_slist_append(NULL, "Content-Type: application/json");
    curl_easy_setopt(self->curl, CURLOPT_HTTPHEADER, self->headers);

    ret = 0;
out:
    return ret;
}

int
gaclient_free(gaclient_t *self)
{

    if (self->base_url != NULL) {
        free(self->base_url);
    }
    if (self->response != NULL) {
        free(self->response);
    }
    if (self->headers != NULL) {
        curl_slist_free_all(self->headers);
    }
    if (self->curl != NULL) {
        curl_easy_cleanup(self->curl);
    }
    return 0;
}

int
gaclient_search_variant_sets(gaclient_t *self)
{

    int ret = GAERR_GENERIC;
    CURLcode curl_ret;
    const char *url_suffix = "/variantsets/search";
    const char *json_request = "{}";
    size_t base_url_len;
    char *url = NULL;

    base_url_len = strlen(self->base_url);
    url = malloc(base_url_len + strlen(url_suffix) + 1);
    if (url == NULL) {
        ret = GAERR_NO_MEMORY;
        goto out;
    }
    strcpy(url, self->base_url);
    strcpy(url + base_url_len, url_suffix);

    curl_easy_setopt(self->curl, CURLOPT_URL, url);
    curl_easy_setopt(self->curl, CURLOPT_POSTFIELDS, json_request);
    curl_easy_setopt(self->curl, CURLOPT_POSTFIELDSIZE, strlen(json_request));
    self->response_length = 0;
    curl_ret = curl_easy_perform(self->curl);
    if (curl_ret != CURLE_OK) {
        ret = curl_ret;
    }

    /* zero terminate the response */
    assert(self->response_length < self->max_response_length);
    self->response[self->response_length] = '\0';
    json_error_t err;
    json_t *root = json_loads(self->response, 0, &err);

    if (root == NULL) {

        printf("ERROR\n");
    }

    if (!json_is_object(root)) {
        printf("Protocol error\n");
    }

    json_decref(root);

    ret = 0;
out:
    if (url != NULL) {
        free(url);
    }
    return ret;
}


int
gaclient_global_init(void)
{
    /* We set this to CURL_GLOBAL_NOTHING to get clean valgrind
     * reports. Using crypto seems to result in loss records.
     */
    curl_global_init(CURL_GLOBAL_NOTHING);
    return 0;
}

int
gaclient_global_free(void)
{
    curl_global_cleanup();
    return 0;
}
