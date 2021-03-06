/* help.h
 * header file for help.c
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 2000-2005 Matthew Hiller
 * 2013 Richard Shann
 */
#ifndef HELP_H
#define HELP_H
void about (GtkAction * action, DenemoScriptParam* param);
void browse_manual (GtkAction * action, DenemoScriptParam* param);
void display_shortcuts (void);
#endif
