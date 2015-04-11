$(srcdir)/deps/libuv/.libs/libuv.a:
	cd $(srcdir)/deps/libuv && touch install-sh && ./autogen.sh && ./configure && make CFLAGS=-fPIC

all: $(srcdir)/deps/libuv/.libs/libuv.a $(all_targets)
	@echo
	@echo "Build complete."
	@echo "Don't forget to run 'make test'."
	@echo
