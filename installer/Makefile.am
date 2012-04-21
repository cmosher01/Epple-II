EXTRA_DIST = epple2.wxs

if HAVE_WINDOWS

SUFFIXES = .wxs .wixobj .msi

all: epple2.msi

epple2.msi: epple2.wixobj ../src/epple2.exe
	strip -o epple2.exe ../src/epple2.exe
	light $< -out $@
	cp $@ $(basename $@)-$(VERSION)$(suffix $@)

epple2.wixobj: epple2.wxs
	candle $< -out $@ -dSDLDIR=/bin -dMINGWMDIR=/mingw/bin

endif
