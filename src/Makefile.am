bin_PROGRAMS = forecast

forecast_SOURCES = forecast.c barplot.c configfile.c network.c render.c cache.c hash.c units.c language.c tree.c lerror.c util.c
forecast_CFLAGS = $(LIBJSONC_CFLAGS) \
									$(LIBCONFIG_CFLAGS) \
									$(LIBCURL_CFLAGS) \
									$(LIBNCURSESW_CFLAGS) \
									$(LIBCRYPTO_CFLAGS) \
									$(LIBLZ4_CFLAGS) \
									$(LIBTALLOC_CFLAGS)
forecast_LDFLAGS = -lm -lpthread \
									 $(LIBJSONC_LIBS) \
									 $(LIBCONFIG_LIBS) \
									 $(LIBCURL_LIBS) \
									 $(LIBNCURSESW_LIBS) \
									 $(LIBCRYPTO_LIBS) \
									 $(LIBLZ4_LIBS) \
									 $(LIBTALLOC_LIBS)
