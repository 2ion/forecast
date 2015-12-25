bin_PROGRAMS = forecast

forecast_SOURCES = forecast.c barplot.c configfile.c network.c render.c cache.c hash.c units.c language.c tree.c
forecast_CFLAGS = $(LIBJSONC_CFLAGS) \
									$(LIBCONFIG_CFLAGS) \
									$(LIBCURL_CFLAGS) \
									$(LIBNCURSESW_CFLAGS) \
									$(LIBCRYPTO_CFLAGS) \
									$(LZ4_CFLAGS) \
									$(TALLOC_CFLAGS)
forecast_LDFLAGS = -lm \
									 $(LIBJSONC_LIBS) \
									 $(LIBCONFIG_LIBS) \
									 $(LIBCURL_LIBS) \
									 $(LIBNCURSESW_LIBS) \
									 $(LIBCRYPTO_LIBS) \
									 $(LZ4_LIBS) \
									 $(TALLOC_LIBS)
