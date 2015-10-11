SUBDIRS = src

man1_MANS = forecast.1

EXTRA_DIST = forecastrc.example README.mkd

dist_doc_DATA = forecastrc.example README.mkd LICENSE

if ENABLE_BASH_COMPLETION
bashcompletiondir = $(BASH_COMPLETION_DIR)
dist_bashcompletion_DATA = shell/forecast.sh
endif
