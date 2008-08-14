## Process this file with automake to produce Makefile.in
docdir   = $(datadir)/covered/doc/docbook/html
doc_DATA = html/chapter.attr.html \
           html/chapter.boundaries.html \
           html/chapter.debug.html \
           html/chapter.epilogue.html \
           html/chapter.faq.html \
           html/chapter.gui.assert.html \
           html/chapter.gui.assert.source.html \
           html/chapter.gui.fsm.html \
           html/chapter.gui.genreport.html \
           html/chapter.gui.intro.html \
           html/chapter.gui.line.html \
           html/chapter.gui.logic.html \
           html/chapter.gui.main.html \
           html/chapter.gui.memory.html \
           html/chapter.gui.new.html \
           html/chapter.gui.preferences.html \
           html/chapter.gui.toggle.html \
           html/chapter.gui.wizard.html \
           html/chapter.installation.html \
           html/chapter.intro.html \
           html/chapter.merge.html \
           html/chapter.metrics.html \
           html/chapter.race.html \
           html/chapter.rank.html \
           html/chapter.reading.html \
           html/chapter.report.html \
           html/chapter.score.html \
           html/chapter.start.html \
           html/chapter.using.html \
           html/index.html \
           html/part.command.line.usage.html \
           html/part.epilogue.html \
           html/part.faq.html \
           html/part.gui.html \
           html/part.installation.html \
           html/part.overview.html

imgdir   = $(datadir)/covered/doc/docbook/html/img
img_DATA = html/img/app_menu.gif \
           html/img/assert_excl.gif \
           html/img/assert_src.gif \
           html/img/assert_window.gif \
           html/img/banner.gif \
           html/img/close_warn.gif \
           html/img/col_show_hide.gif \
           html/img/color.gif \
           html/img/comb_complex.gif \
           html/img/comb_event.gif \
           html/img/comb_simple.gif \
           html/img/comb_simple_and.gif \
           html/img/comb_simple_or.gif \
           html/img/comb_unary.gif \
           html/img/dn_button.gif \
           html/img/exit_warn.gif \
           html/img/file_menu.gif \
           html/img/file_viewer.gif \
           html/img/fsm_excl.gif \
           html/img/fsm_window.gif \
           html/img/gen_menu.gif \
           html/img/help_menu.gif \
           html/img/home.gif \
           html/img/line_excl.gif \
           html/img/line_incl.gif \
           html/img/main_cov.gif \
           html/img/main_info.gif \
           html/img/main_lb.gif \
           html/img/main_menu.gif \
           html/img/main_viewer.gif \
           html/img/main_window.gif \
           html/img/memory_full.gif \
           html/img/new_cdd_define.gif \
           html/img/new_cdd_fsm.gif \
           html/img/new_cdd_gen.gif \
           html/img/new_cdd_insert_menu.gif \
           html/img/new_cdd_lib_ext.gif \
           html/img/new_cdd_mod_excl.gif \
           html/img/new_cdd_mod_gen.gif \
           html/img/new_cdd_name.gif \
           html/img/new_cdd_options.gif \
           html/img/new_cdd_options2.gif \
           html/img/new_cdd_parm_oride.gif \
           html/img/new_cdd_selection.gif \
           html/img/new_cdd_type.gif \
           html/img/next.gif \
           html/img/note.gif \
           html/img/open_cdd.gif \
           html/img/populated_lb.gif \
           html/img/pref_color.gif \
           html/img/pref_goals.gif \
           html/img/pref_main.gif \
           html/img/pref_syntax.gif \
           html/img/prev.gif \
           html/img/report_menu.gif \
           html/img/rpt_gen_view.gif \
           html/img/rpt_gen_selection.gif \
           html/img/rpt_gen_options.gif \
           html/img/toggle_full.gif \
           html/img/up.gif \
           html/img/up_button.gif \
           html/img/view_menu.gif \
           html/img/wizard.gif

mk-html:   
	XML_CATALOG_FILES="catalog.xml" xsltproc \
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
                --stringparam base.dir html/ \
                covered.xsl covered_doc.xml
           
