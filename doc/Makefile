.SUFFIXES: .asciidoc .html

.asciidoc.html :
	asciidoc -b html5 $<

all : doc

doc : index.html

clean :
	rm -f index.html

install :
	mkdir -p deploy
	cp *.html *.png *.gif deploy
	cp -R callouts deploy

uninstall :
	rm -Rf deploy
