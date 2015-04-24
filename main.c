/*
 * Simple runner for the client API.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ga4gh.h"

static void
fatal_error(const char *msg, ...)
{
    va_list argp;
    fprintf(stderr, "sms:");
    va_start(argp, msg);
    vfprintf(stderr, msg, argp);
    va_end(argp);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}


int
main(int argc, char** argv)
{
    int err, j;
    printf("main\n");
    gaclient_t *client = malloc(sizeof(gaclient_t));
    if (client == NULL) {
        fatal_error("Out of memory");
    }
    err = gaclient_global_init();
    if (err != 0) {
        fatal_error("Error: %d\n", err);
    }
    err = gaclient_init(client, "http://localhost:8000/v0.5.1");
    if (err != 0) {
        fatal_error("Error: %d\n", err);
    }
    for (j = 0; j < 1; j++) {
        printf("HERE!n\n");
        err = gaclient_search_variant_sets(client);
        if (err != 0) {
            fatal_error("Error: %d\n", err);
        }
    }
    err = gaclient_free(client);
    if (err != 0) {
        fatal_error("Error: %d\n", err);
    }
    free(client);
    err = gaclient_global_free();
    if (err != 0) {
        fatal_error("Error: %d\n", err);
    }
    return EXIT_SUCCESS;
}
