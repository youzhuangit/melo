# Libraries
melolib_LTLIBRARIES = libmelo.la

# Main library
libmelo_la_SOURCES = \
	melo_plugin.c \
	melo_config.c \
	melo_module.c \
	melo_browser.c \
	melo_player.c \
	melo_playlist.c \
	melo_tags.c \
	melo_config_jsonrpc.c \
	melo_module_jsonrpc.c \
	melo_browser_jsonrpc.c \
	melo_player_jsonrpc.c \
	melo_playlist_jsonrpc.c \
	melo_avahi.c \
	melo_rtsp.c \
	melo_jsonrpc.c

libmelo_la_CFLAGS = $(MELO_ALL_CFLAGS)
libmelo_la_LIBADD = $(MELO_ALL_LIBS)

meloincludedir = $(includedir)/melo
meloinclude_HEADERS = \
	melo_plugin.h \
	melo_config.h \
	melo_module.h \
	melo_browser.h \
	melo_player.h \
	melo_playlist.h \
	melo_tags.h \
	melo_config_jsonrpc.h \
	melo_module_jsonrpc.h \
	melo_browser_jsonrpc.h \
	melo_player_jsonrpc.h \
	melo_playlist_jsonrpc.h \
	melo_avahi.h \
	melo_rtsp.h \
	melo_jsonrpc.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libmelo.pc
EXTRA_DIST = libmelo.pc.in
