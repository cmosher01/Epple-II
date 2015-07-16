SUFFIXES = .asciidoc .html

.asciidoc.html :
	asciidoc -b html5 -a VERSION=@VERSION@ $<



.PHONY : html install-html clean

html : index.html

index.html : *.asciidoc

install-html :
	mkdir -p $(htmldir)
	cp -R *.html *.png *.gif callouts $(htmldir)

clean :
	rm -f index.html
