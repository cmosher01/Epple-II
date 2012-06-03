.SUFFIXES: .asciidoc .html

.asciidoc.html :
	asciidoc -b html5 $<

all : doc

doc : index.html

index.html : *.asciidoc

clean :
	rm -f index.html

.PHONY: site

site :
	mkdir -p site
	cp *.html *.png *.gif site
	cp -R callouts site
