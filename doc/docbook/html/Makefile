# DOCBOOK_DSL = /sw/share/sgml/dsssl/docbook-dsssl-nwalsh/html/docbook.dsl
DOCBOOK_DSL = /sw/share/sgml/dsssl/docbook-dsssl-nwalsh/html/ldp.dsl

multiple: clean
	-openjade -t sgml -d $(DOCBOOK_DSL) ../covered_doc.xml

single: clean
	-openjade -t sgml -V nochunks -d $(DOCBOOK_DSL) ../covered_doc.xml > index.html

clean:
	rm -f *.htm *.html
