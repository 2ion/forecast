bin_PROGRAMS = forecast

forecast_SOURCES = forecast.c barplot.c
forecast_CFLAGS = $(LIBJSONC_CFLAGS) \
									$(LIBCONFIG_CFLAGS) \
									$(LIBCURL_CFLAGS) \
									$(LIBNCURSESW_CFLAGS)
forecast_LDFLAGS = -lm \
									 $(LIBJSONC_LIBS) \
									 $(LIBCONFIG_LIBS) \
									 $(LIBCURL_LIBS) \
									 $(LIBNCURSESW_LIBS)
