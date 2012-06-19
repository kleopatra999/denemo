//      scorelayout.h
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





#ifndef scorelayout_H
#define scorelayout_H

#include <denemo/denemo.h> 
void  create_default_scoreblock(void);
void  free_scoreblocks(DenemoGUI *gui);
DenemoScoreblock *select_layout(gboolean all_movements, gchar *partname);
DenemoScoreblock *get_scoreblock_for_lilypond(gchar *lily);
GtkWidget *get_score_layout_notebook(DenemoGUI *gui);
gboolean create_custom_scoreblock (gchar *layout_name);
DenemoScoreblock *selected_scoreblock(void);
guint selected_layout_id(void);
#endif