/* drawfigure.cpp 
 *
 * function to display Figured Bass figures on score
 * for Denemo, a gtk+ frontend to GNU Lilypond 
 * 
 * (c) 2003-2005 Richard Shann <richard.shann@virgin.net>
 */

#include "core/utils.h"
#include <denemo/denemo.h>
#include <string.h>

/**
 * Draw figured bass on the score
 *
 */
void
draw_figure (cairo_t * cr, gint xx, gint y, DenemoObject * theobj)
{
  chord *ch;
  if (theobj->type == CHORD)
    {
      GString *gstr = g_string_new ("");
      ch = (chord *) theobj->object;
      gint ystep = 0, xstep = 0;
      gint accs = FALSE;
      gchar *orig = ((GString *) (ch->figure))->str;
      gchar *str = orig;
      while (*str)
        {
          if (*str == '+')
            {
              accs = TRUE, g_string_append (gstr, "♯");
            }
          else if (*str == '-')
            {
              accs = TRUE, g_string_append (gstr, "♭");
            }
          else if (*str == '!')
            {
              accs = TRUE, g_string_append (gstr, "♮");
            }
          else if (*str == '_')
            {
              /*do nothing */ ;
            }
          else if (*str == '|')
            {
              drawlargetext_cr (cr, gstr->str, xx + xstep, y + STAFF_HEIGHT + 20 + ystep);
              ystep = 0;
              xstep += 25 + (accs ? 10 : 0);
              g_string_assign (gstr, "");
              accs = FALSE;
            }
          else if (*str == ' ')
            {
              if (gstr->len)
                {
                  drawlargetext_cr (cr, gstr->str, xx + xstep, y + STAFF_HEIGHT + 20 + ystep);
                  ystep += 20;
                  g_string_assign (gstr, "");
                }
            }
          else
            {
              g_string_append_c (gstr, *str);
            }
          str++;
          if (*str == 0)
            {
              drawlargetext_cr (cr, gstr->str, xx + xstep, y + STAFF_HEIGHT + 20 + ystep);
              break;
            }
        }
      g_string_free (gstr, TRUE);
    }
  //g_debug ("%s\n", text);
}
