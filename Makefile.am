SUBDIRS = src conf installer
EXTRA_DIST = COPYING.sdl COPYING.other

.PHONY: site doc

site: doc

doc:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ site
