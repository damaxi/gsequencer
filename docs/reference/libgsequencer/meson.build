# Copyright (C) 2020 Joël Krähemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

fs = import('fs')

private_headers = []
content_files = []
images = []

types_conf = configuration_data()

if get_option('gtk_doc')
  configure_file(input: 'version.xml.in', output: 'version.xml', configuration: version_conf)

  gnome.gtkdoc('libgsequencer',
               mode: 'none',
               main_xml: 'libags.xml',
               src_dir: [
                 includes,
               ],
               dependencies: libgsequencer,
               gobject_typesfile: configure_file(
                 input: 'libgsequencer.types.in',
                 output: 'libgsequencer.types',
                 configuration: types_conf,
               ),
               scan_args: [
                 '--ignore-decorators=_GDK_EXTERN|G_GNUC_WARN_UNUSED_RESULT',
                 '--ignore-headers=' + ' '.join(private_headers),
               ],
               mkdb_args: [
                 '--default-includes=gtk/gtk.h',
               ],
               fixxref_args: [
                 '--html-dir=@0@'.format(docpath),
                 '--extra-dir=@0@'.format(join_paths(glib_docpath, 'glib')),
                 '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gobject')),
                 '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gio')),
                 '--extra-dir=@0@'.format(cairo_docpath),
                 '--extra-dir=@0@'.format(gdkpixbuf_docpath),
                 '--extra-dir=@0@'.format(gtk_docpath),
		 '--extra-dir=../libags',
		 '--extra-dir=../libags-audio',
		 '--extra-dir=../libags-gui',
               ],
               content_files: content_files,
               html_assets: images,
               install: true)
endif
