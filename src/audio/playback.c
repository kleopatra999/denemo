/* playback.c
 * Midi playback for a specific portion of
 * a score
 *
 * (c) 2000-2005 Adam Tee
 */


#include <denemo/denemo.h>
#include "audio/playback.h"
#include "export/exportlilypond.h"
#include "export/exportmidi.h"
#include "command/staff.h"
#include "command/score.h"
#include "ui/dialogs.h"
#include "core/prefops.h"
#include "core/utils.h"
#include "core/external.h"
#include "audio/audiointerface.h"
#include "audio/midi.h"
#include "core/view.h"
#include "source/sourceaudio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_WAIT_H
#include <wait.h>
#endif
#include <errno.h>

void
set_tempo (void)
{
  gdouble tempo = Denemo.project->movement->master_tempo;
  if (tempo < 0.001 || (tempo > 0.999 && tempo < 1.001))
    return;
  Denemo.project->movement->tempo *= tempo;
  Denemo.project->movement->start_time /= tempo;
  Denemo.project->movement->end_time /= tempo;

  Denemo.project->movement->master_tempo = 1.0;
  score_status (Denemo.project, TRUE);
  exportmidi (NULL, Denemo.project->movement, 0, 0);
}


/* start playing the current movement as MIDI
 * the name ext_... is anachronistic, Fluidsynth or Jack are normally used.
 */
void
ext_midi_playback (GtkAction * action, DenemoScriptParam * param)
{
  if(Denemo.non_interactive)
    return;

  GET_1PARAM (action, param, callback);

  set_playbutton (is_paused ());
  if (is_playing ())
    {
      toggle_paused ();
      return;
    }
  set_tempo ();

  //rewind_audio(); done in start_audio_playing()
  start_audio_playing (FALSE);
  midi_play (callback);
}


// a function that stops any play in progress, and starts play again.
void
restart_play (void)
{
  stop_midi_playback (NULL, NULL);
  ext_midi_playback (NULL, NULL);
}

void
stop_midi_playback (GtkAction * action, DenemoScriptParam* param)
{

  if (is_paused ())
    toggle_paused ();
  else
    set_playbutton (TRUE);
  midi_stop ();

  draw_score_area();     //update playhead on screen
}

void
playback_panic ()
{
  panic_all ();
}

/** 
 * Dialog function used to select measure range 
 * This is similar to printrangedialog in print.c
 */

void
PlaybackRangeDialog ()
{
  DenemoProject *gui = Denemo.project;
  GtkWidget *dialog;
  GtkWidget *label;
  GtkWidget *hbox;
  GtkWidget *from_time;
  GtkWidget *to_time;

  dialog = gtk_dialog_new_with_buttons (_("Play range in seconds:"), GTK_WINDOW (Denemo.window), (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT), GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

  hbox = gtk_hbox_new (FALSE, 8);

  GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  gtk_container_add (GTK_CONTAINER (content_area), hbox);

  //TODO calculate hightest number in seconds
  gdouble max_end_time = 7200.0;
  //g_list_length (((DenemoStaff *) (gui->movement->thescore->data))->measures);

  label = gtk_label_new (_("Play from time"));
  gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

  from_time = gtk_spin_button_new_with_range (0.0, max_end_time, 0.1);
  gtk_box_pack_start (GTK_BOX (hbox), from_time, TRUE, TRUE, 0);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (from_time), (gdouble) gui->movement->start_time);

  label = gtk_label_new (_("to"));
  gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

  to_time = gtk_spin_button_new_with_range (0.0, max_end_time, 0.1);
  gtk_box_pack_start (GTK_BOX (hbox), to_time, TRUE, TRUE, 0);

  gtk_spin_button_set_value (GTK_SPIN_BUTTON (to_time), (gdouble) gui->movement->end_time);

  gtk_widget_show (hbox);
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_MOUSE);
  gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
  gtk_widget_show_all (dialog);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      gui->movement->start_time = gtk_spin_button_get_value (GTK_SPIN_BUTTON (from_time));
      gui->movement->end_time = gtk_spin_button_get_value (GTK_SPIN_BUTTON (to_time));
      //gtk_widget_destroy (dialog);
    }

  gtk_widget_destroy (dialog);
}
