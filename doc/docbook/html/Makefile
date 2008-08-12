# DOCBOOK_DSL = /sw/share/sgml/dsssl/docbook-dsssl-nwalsh/html/docbook.dsl
DOCBOOK_DSL = /sw/share/sgml/dsssl/docbook-dsssl-nwalsh/html/ldp.dsl
DOCBOOK_XSL = /home/bryce3/trevorw/Desktop/docbook-xsl-1.74.0/html/chunkfast.xsl

all:
	xsltproc \
		--stringparam draft.mode no \
		--stringparam section.autolabel 1 \
		--stringparam section.label.includes.component.label 1 \
		--stringparam use.extensions 0 \
		--stringparam chunk.section.depth 0 \
		--stringparam chunk.first.sections 1 \
		--stringparam use.id.as.filename 1 \
		--stringparam admon.graphics.path img/ \
		--stringparam admon.graphics.extension .gif \
		--stringparam admon.graphics 1 \
		--stringparam navig.graphics.path img/ \
		--stringparam navig.graphics.extension .gif \
		--stringparam navig.graphics 1 \
		$(DOCBOOK_XSL) ../covered_doc.xml

clean:
	rm -f *.htm *.html
