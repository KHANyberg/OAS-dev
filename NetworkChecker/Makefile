LIBS=$(shell pkg-config --libs gio-2.0 gobject-2.0 glib-2.0)
CFLAGS=$(shell pkg-config --cflags gio-2.0)

all:
	gcc $(CFLAGS) -o networkchecker_gdbus networkchecker_gdbus.c $(LIBS)