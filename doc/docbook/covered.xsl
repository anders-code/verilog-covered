<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:exsl="http://exslt.org/common"
                xmlns:cf="http://docbook.sourceforge.net/xmlns/chunkfast/1.0"
                version="1.0"
                exclude-result-prefixes="cf exsl">
  
<!-- ********************************************************************
     $Id$
     ********************************************************************

     This file is part of the XSL DocBook Stylesheet distribution.
     See ../README or http://nwalsh.com/docbook/xsl/ for copyright
     and other information.

     ******************************************************************** -->

<!-- ==================================================================== -->

  <xsl:import href="chunkfast.xsl"/>

  <xsl:template name="body.attributes">
    <xsl:attribute name="bgcolor">#d8e9f0</xsl:attribute>
    <xsl:attribute name="text">black</xsl:attribute>
    <xsl:attribute name="link">#0000FF</xsl:attribute>
    <xsl:attribute name="vlink">#840084</xsl:attribute>
    <xsl:attribute name="alink">#0000FF</xsl:attribute>
  </xsl:template>

</xsl:stylesheet>
