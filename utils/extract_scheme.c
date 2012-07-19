//      extract_scheme.c
//      
//      Copyright 2012 Richard Shann <rshann@debian2>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

//gcc extract_scheme.c -o extract_scheme -pthread -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -lxml2 -lglib-2.0 -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lpangoft2-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lcairo -lgio-2.0 -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0

//pkg-config --cflags glib-2.0
//pkg-config --libs glib-2.0
//pkg-config --libs libxml-2.0
//pkg-config --cflags libxml-2.0
#include <stdio.h>
#include <glib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
static void
extract_scheme (xmlDocPtr doc, xmlNodePtr cur, gchar *filename)
{
  cur = cur->xmlChildrenNode;
  while (cur != NULL)
    {
    if (0 == xmlStrcmp (cur->name, (const xmlChar *) "scheme")) {
				gchar *scheme =  xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
        if(scheme && *scheme) {
          g_file_set_contents(filename, scheme, -1, NULL);
          g_print("Written %s\n", filename);
        }  
      }
      cur = cur->next;
    }
}
static gchar *usage = "extract_scheme commandfile_in_xml commandscripts-directory-path";
int main(int argc, char **argv)
{
  if(argc!=2) {
      g_critical("%s: wrong number of arguments", usage);
    return -1;
  }
  gchar * filename = argv[1];
  gchar *commandscripts = argv[2];
  gint ret = 0;
  xmlDocPtr doc;
  xmlNodePtr rootElem;
  if(!g_file_test(commandscripts, G_FILE_TEST_EXISTS)) {
      g_critical("%s: directory %s commandscripts does not exist", usage, commandscripts);
    return -1;
  }
  if(!g_file_test(commandscripts, G_FILE_TEST_IS_DIR)) {
      g_critical("%s: directory %s commandscripts is not a directory", usage, commandscripts);
    return -1;
  }
  if(!g_file_test(filename, G_FILE_TEST_EXISTS))
    return ret;
  doc = xmlParseFile (filename);
  if (doc == NULL) {
    return ret;
  }
  rootElem = xmlDocGetRootElement (doc);
  if (rootElem == NULL) {
    xmlFreeDoc (doc);
    return ret;
  }
  if (xmlStrcmp (rootElem->name, (const xmlChar *) "Denemo"))
    {
      xmlFreeDoc (doc);
      return ret;
    }
  rootElem = rootElem->xmlChildrenNode;
	filename = g_strconcat(commandscripts, "/", filename, ".scm", NULL);
  while (rootElem != NULL) {
    if ( (0 == xmlStrcmp (rootElem->name, (const xmlChar *) "merge"))) {
      xmlNodePtr cur;
      for (cur = rootElem->xmlChildrenNode; cur != NULL;  cur = cur->next) {
				if (0 == xmlStrcmp (cur->name, (const xmlChar *) "map")) {
					xmlNodePtr ncur;
					for(ncur=cur->xmlChildrenNode;ncur != NULL; ncur = ncur->next) {
						extract_scheme (doc, ncur, filename);
					}				
				}
      } 
    }
    rootElem = rootElem->next;
  }
  xmlFreeDoc (doc);
  return ret;

}

