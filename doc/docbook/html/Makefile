DOCBOOK_DSL = /sw/share/sgml/dsssl/docbook-dsssl-nwalsh/html/docbook.dsl

all:    clean
	openjade -t sgml -V nochunks -d $(DOCBOOK_DSL) ../covered_doc.xml > index.html

clean:
	rm -f *.htm *.html
