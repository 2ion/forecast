bin_PROGRAMS = forecast forecastfs

forecast_SOURCES = forecast.c barplot.c configfile.c network.c render.c cache.c hash.c units.c language.c tree.c
forecast_CFLAGS = $(LIBJSONC_CFLAGS) \
									$(LIBCONFIG_CFLAGS) \
									$(LIBCURL_CFLAGS) \
									$(LIBNCURSESW_CFLAGS) \
									$(LIBCRYPTO_CFLAGS) \
									$(LIBLZ4_CFLAGS) \
									$(LIBTALLOC_CFLAGS)
forecast_LDFLAGS = -lm \
									 $(LIBJSONC_LIBS) \
									 $(LIBCONFIG_LIBS) \
									 $(LIBCURL_LIBS) \
									 $(LIBNCURSESW_LIBS) \
									 $(LIBCRYPTO_LIBS) \
									 $(LIBLZ4_LIBS) \
									 $(LIBTALLOC_LIBS)

forecastfs_SOURCES = fs.c configfile.c network.c cache.c hash.c units.c language.c tree.c
forecastfs_CFLAGS = $(LIBFUSE_CFLAGS) \
										$(LIBCURL_CFLAGS) \
										$(LIBJSONC_CFLAGS) \
										$(LIBTALLOC_CFLAGS) \
										$(LIBCONFIG_CFLAGS) \
										$(LIBCRYPTO_CFLAGS) \
										$(LIBLZ4_CFLAGS)
forecastfs_LDFLAGS = -lm \
										 $(LIBFUSE_LIBS) \
										 $(LIBCURL_LIBS) \
										 $(LIBJSONC_LIBS) \
										 $(LIBTALLOC_LIBS) \
										 $(LIBCONFIG_LIBS) \
										 $(LIBCRYPTO_LIBS) \
										 $(LIBLZ4_LIBS)
