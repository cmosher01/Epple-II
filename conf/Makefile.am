etcdir = $(sysconfdir)/epple2

if HAVE_WINDOWS
CONF_PREFIX =
else
CONF_PREFIX = $(prefix)/
endif

%.conf: %.conf.in
	sed -e 's,$$(PREFIX),$(CONF_PREFIX),g' <$< >$@

CONF_FILES = \
    epple2.conf \
    epple2.a2bare.conf \
    epple2.a2dos33.conf \
    epple2.a2pbare.conf \
    epple2.a2ploaded.conf \
    epple2.rev0bare.conf \
    epple2.a2dos31.conf \
    epple2.a2loaded.conf \
    epple2.a2pdos33.conf

etc_DATA = $(CONF_FILES)

EXTRA_DIST = $(etc_DATA)
