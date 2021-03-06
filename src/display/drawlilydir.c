/* drawlilydir.cpp
 *
 * Functions for drawing stemming directives
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 1999, 2000, 2001, 2002 Adam Tee, 2008, 2009 Richard Shann
 */

#include "core/utils.h"              /* Includes <gdk.h> */
#include "command/scorelayout.h"
#include <denemo/denemo.h>
#include <string.h>
#include <math.h>
/**
 * Draw a standalone Denemo directive on the score as a vertical bar or graphic and/or text
 *
 */
void
draw_lily_dir (cairo_t * cr, gint xx, gint y, gint highy, gint lowy, DenemoObject * theobj, gboolean selected, gboolean at_cursor)
{
  DenemoDirective *lily = ((lilydirective *) theobj->object);
  gchar *first = (lily->postfix && lily->postfix->len) ? lily->postfix->str : " ";
  guint layout = selected_layout_id ();
  gdouble only = lily->y ? ((lily->y == layout) ? 0.5 : 0.0) : 0.0;
  gdouble exclude = lily->x ? ((lily->x == layout) ? 0.9 : 0.0) : 0.0;
  if (lily->y && lily->y != layout)
    exclude = 0.9;
  cairo_save (cr);
  
  selected ? cairo_set_source_rgba (cr, 0.0, 0.0, 1.0, at_cursor ? 1.0 : 0.5) : lily->graphic ? cairo_set_source_rgb (cr, 0.0 + exclude, 0.0 + only, 0.0) : cairo_set_source_rgba (cr, 0.4 + exclude, 0.5 + only, 0.4, at_cursor ? 1.0 : 0.5);
  if (lily->graphic)
    {
      //FIXME there may be scripts expecting a different positioning code
      gdouble gx = xx + lily->gx - (((DenemoGraphic *) lily->graphic)->width) / 2;
      gdouble gy = y + MID_STAFF_HEIGHT + lily->gy - (((DenemoGraphic *) lily->graphic)->height) / 2;

      drawbitmapinverse_cr (cr, (DenemoGraphic *) lily->graphic, gx, gy, FALSE);
    }
 // else instead always show position of standalone directive
    {
        at_cursor?
      cairo_set_source_rgba (cr, 0.2 + exclude, 0.3 + only, 0.8, 0.5):
      cairo_set_source_rgba (cr, 0.4 + exclude, 0.5 + only, 0.4, 0.5);
      
      
      cairo_rectangle (cr, xx+10, y - 20, 2, STAFF_HEIGHT + 26);
      cairo_arc (cr, xx+10 + 1.5, y - 20, 6, 0.0, 2 * M_PI);
      cairo_fill (cr);
      cairo_move_to (cr, xx+10, y - 20);
      cairo_line_to (cr, xx+10 + lily->gx, y + MID_STAFF_HEIGHT + lily->gy);
      cairo_stroke (cr);
    }
  if (lily->display)
    {                           //store display position x,y as well
#define MAXLEN (4)
      gchar c = 0;              //if it is a long string only show it all when cursor is on it, also only display from first line
      gchar *p;
      for (p = lily->display->str; *p; p = g_utf8_next_char(p))
        {
          if (*p == '\n' || (!at_cursor && (p - lily->display->str) > MAXLEN))
            {
              c = *p;
              *p = 0;
              break;
            }
        }
      drawnormaltext_cr (cr, lily->display->str, xx + lily->tx, y + lowy + lily->ty - 8);
      if (c)
        {
          *p = c;
        }
    }
  else
    //FIXME do this by creating a display field
  if ((!lily->graphic) && (*first == '%' || *first == '^' || *first == '_'))
    {                           //display comments, and markup above and below
      drawnormaltext_cr (cr, first + 1, xx, *first == '_' ? y + lowy + 20 : y - highy - 20);
    }
  cairo_restore (cr);
}
