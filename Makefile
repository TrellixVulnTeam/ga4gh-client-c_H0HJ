CURL_CFLAGS := $(shell curl-config --cflags)
CURL_LDFLAGS := $(shell curl-config --libs)
JANSSON_CFLAGS := $(shell pkg-config --cflags jansson)
JANSSON_LDFLAGS := $(shell pkg-config --libs jansson)
CFLAGS=-Wall -g -O3
#CFLAGS=-Wall -g -O3 -march=native 
CFLAGS=-std=c99 -pedantic -Werror -Wall -W \
  -Wno-unused-parameter\
  -Wmissing-prototypes -Wstrict-prototypes \
  -Wconversion -Wshadow -Wpointer-arith \
  -Wcast-qual -Wcast-align \
  -Wwrite-strings -Wnested-externs \
  -fshort-enums -fno-common -Dinline= -g -O2 \
  ${CURL_CFLAGS} ${JANSSON_CFLAGS}
LDFLAGS=${CURL_LDFLAGS} ${JANSSON_LDFLAGS}

main: ga4gh.c main.c ga4gh.h
	gcc ${CFLAGS} -o main ga4gh.c main.c ${LDFLAGS} 
	
