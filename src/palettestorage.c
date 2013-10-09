/*
 * palettestorage.c
 * 
 * Copyright 2013 Richard Shann <rshann@virgin.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */
#include "palettes.h"
#include "view.h"
#include "utils.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>

static save_button (xmlNodePtr button, GtkWidget *widget)
{
	//newXMLIntProp (xmlNodePtr parent, const xmlChar * name, gint content)
	gchar *label = g_object_get_data (G_OBJECT(widget), "icon");
	if(label == NULL)
		label = gtk_button_get_label(GTK_BUTTON(widget));
	xmlSetProp (button, (xmlChar *) "_label", (xmlChar *) label );
	xmlSetProp (button, (xmlChar *) "_tooltip", (xmlChar *) gtk_widget_get_tooltip_text(widget));
	xmlSetProp (button, (xmlChar *) "script", (xmlChar *) g_object_get_data (G_OBJECT(widget), "script"));
}
/**
 * 
Set a prop of the parent, holding the passed name and integer.
 */
static void
newXMLIntProp (xmlNodePtr parent, const xmlChar * name, gint content)
{
  gchar *integer = g_strdup_printf ("%d", content);
  xmlSetProp (parent, name, (xmlChar *) integer);
  g_free (integer);
}


static save_palette (xmlNodePtr parent, DenemoPalette *pal)
{
	xmlSetProp (parent, (xmlChar *) "_name", (xmlChar *) pal->name);

	newXMLIntProp (parent, "row-wise", pal->rows);
	newXMLIntProp (parent, "limit", pal->limit);
	newXMLIntProp (parent, "dock", pal->docked);

	newXMLIntProp (parent, "hidden", pal->docked?!gtk_widget_get_visible(pal->box): !gtk_widget_get_visible(pal->window));
	GList *g;
	for(g=pal->buttons;g;g=g->next)
	{
		xmlNodePtr child = xmlNewChild (parent, NULL, (xmlChar *) "button", NULL);
		save_button (child, g->data);
	}
	
}



gint
writePalettes (void)
{
  gint ret = -1;
  xmlDocPtr doc;
  xmlNodePtr parent, child;
  gchar *localpal = NULL;
  
  localpal = g_build_filename (get_user_data_dir (), "actions", "palettes.xml", NULL);

  doc = xmlNewDoc ((xmlChar *) "1.0");
  doc->xmlRootNode = parent = xmlNewDocNode (doc, NULL, (xmlChar *) "Denemo", NULL);
	GList *g;
	for( g = Denemo.palettes; g; g = g->next) 
		{	
		child = xmlNewChild (parent, NULL, (xmlChar *) "palette", NULL);
		save_palette (child, g->data);
		}
	if (xmlSaveFormatFile (localpal, doc, 1) < 0)
    {
      g_warning ("Could not save file %s", localpal);
      ret = -1;
    } else
    ret = 0;
	xmlFreeDoc (doc);
	return ret;
}	
	
	
	
/**
 * install palettes from file
 
 */
 
static gint getXMLIntProp (xmlNodePtr child, gchar *name)
 {
  gint val;
  gchar *thename = (gchar *) xmlGetProp (child, (xmlChar *) name);
  if (thename)
    val = atoi (thename);
    xmlFree (thename);
    return val;
}


#define FOREACH_CHILD_ELEM(childElem, parentElem) \
for ((childElem) = (parentElem)->xmlChildrenNode; \
     (childElem) != NULL; \
     (childElem) = (childElem)->next)

#define ELEM_NAME_EQ(childElem, childElemName) \
(strcmp ((gchar *)(childElem)->name, (childElemName)) == 0)

 static void installButtons (xmlNodePtr palette, DenemoPalette *pal)
 {
	xmlNodePtr childElem;

  FOREACH_CHILD_ELEM (childElem, palette) 
  if (ELEM_NAME_EQ (childElem, "button"))
  {
	gchar *label = (gchar *) xmlGetProp (childElem, (xmlChar *) "_label");	
	gchar *tooltip = (gchar *) xmlGetProp (childElem, (xmlChar *) "_tooltip");	
	gchar *script = (gchar *) xmlGetProp (childElem, (xmlChar *) "script");	
	if(label && tooltip && script)
		palette_add_button (pal, label, tooltip, script);
	else 
		g_warning ("Bad value for button in palettes.xml %s %s %s\n", label, tooltip, script);
  }
	 
	 
 }
    
static void install_palette (xmlNodePtr palette) 
{
	gchar *name = (gchar *) xmlGetProp (palette, (xmlChar *) "_name");	
	gboolean hidden =  getXMLIntProp (palette, (xmlChar *) "hidden");
	gboolean row_wise =  getXMLIntProp (palette, (xmlChar *) "row-wise");
	gboolean dock =  getXMLIntProp (palette, (xmlChar *) "dock");
	gint limit =  getXMLIntProp (palette, (xmlChar *) "limit");
	DenemoPalette *pal = create_palette (name, dock, row_wise);
	set_palate_shape (name, row_wise, limit);//does gtk_widget_show in repack	
	installButtons (palette, pal);
	if(hidden)
		gtk_widget_hide(pal->docked?pal->box:pal->window);
	else
		gtk_widget_show(pal->docked?pal->box:pal->window);
		
	if (pal->buttons==NULL)
			{
				delete_palette (pal);
			}
}
gint
installPalettes (void)
{
  xmlDocPtr doc = NULL;
  xmlNodePtr rootElem;

  gchar *filename = NULL;
  filename = g_build_filename (get_user_data_dir (), "actions", "palettes.xml", NULL);
  if(!g_file_test (filename, G_FILE_TEST_EXISTS))
	{
			g_free(filename);
			filename = NULL;
	}
  
  if (filename == NULL)
    filename = g_build_filename (get_system_data_dir (), "actions", "palettes.xml", NULL);

  doc = xmlParseFile (filename);
  if (doc == NULL)
    {
      g_warning ("Could not read XML file %s", filename);
      return -1;
    }

  rootElem = xmlDocGetRootElement (doc);
  if (rootElem == NULL)
    {
      g_warning ("Empty Document\n");
      xmlFreeDoc (doc);
      return -1;
    }

  g_print ("RootElem: %s\n", rootElem->name);
  if (0 != xmlStrcmp (rootElem->name, (xmlChar*) "Denemo"))
    {
      g_warning ("Document has wrong type\n");
      xmlFreeDoc (doc);
      return -1;
    }

  rootElem = rootElem->xmlChildrenNode;
  while (rootElem != NULL)
    {
      g_print ("RootElem %s\n", rootElem->name);
      if (0 == xmlStrcmp (rootElem->name, (const xmlChar *) "palette"))
        {         
		  install_palette (rootElem);
        }
      rootElem = rootElem->next;
    }

  xmlFreeDoc (doc);
  return 0;
}



