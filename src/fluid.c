#ifdef _HAVE_FLUIDSYNTH_
#include <glib.h>
#include <fluidsynth.h>
#include <sys/time.h>
#include "midi.h"
#include "smf.h"
#include "fluid.h"

#define MAX_NUMBER_OF_TRACKS    128
#define NOTE_OFF                0x80
#define NOTE_ON                 0x90
#define KEY_PRESSURE      	0xA0
#define CONTROL_CHANGE  	0xB0
#define PROGRAM_CHANGE  	0xC0
#define	CHANNEL_PRESSURE 	0xD0
#define PITCH_BEND      	0xE0
#define MIDI_SYSTEM_RESET       0xFF

/*********************
NOTE_OFF = 0x80,
46	  NOTE_ON = 0x90,
47	  KEY_PRESSURE = 0xa0,
48	  CONTROL_CHANGE = 0xb0,
49	  PROGRAM_CHANGE = 0xc0,
50	  CHANNEL_PRESSURE = 0xd0,
51	  PITCH_BEND = 0xe0,
case CONTROL_CHANGE:
         return fluid_synth_cc(synth, chan, event->midi_buffer[1], event->midi_buffer[2]); 
                             
       case PROGRAM_CHANGE:
         return fluid_synth_program_change(synth, chan,  event->midi_buffer[1]);
 
       case CHANNEL_PRESSURE:
       return fluid_synth_channel_pressure(synth, chan,  event->midi_buffer[1]);
 
       case PITCH_BEND:
         return fluid_synth_pitch_bend(synth, chan, event->midi_buffer[1] + (event->midi_buffer[2]<<8
				      );
 
       case MIDI_SYSTEM_RESET:
***********************/




#define SYS_EXCLUSIVE_MESSAGE1  0xF0

static fluid_settings_t* settings;
static fluid_synth_t* synth;
static fluid_audio_driver_t* adriver;
static double start_player = 0.0;

/*related to moving the cursor along with playback*/
static double start_time = 0.0;
static double end_time = 0.0;//time in seconds to end at (from start of the smf)
static gdouble playback_duration = 0.0;
static gint timeout_id = 0, kill_id=0;


static gboolean playing_piece;




static double get_time(void)
{
  double          seconds;
  int             ret;
  struct timeval  tv;

  ret = gettimeofday(&tv, NULL);
  if (ret) {
    perror("gettimeofday");
  }
  seconds = tv.tv_sec + tv.tv_usec / 1000000.0;
  return seconds;
}




static gint start_fluid_settings()
{
  settings = new_fluid_settings();
  if (!settings){
    g_warning("\nFailed to create the settings\n");
    fluidsynth_shutdown();
    return 1;
  }
  return 0;
}

gchar * fluidsynth_get_default_audio_driver()
{
  if (!settings)
    start_fluid_settings(); 
  return fluid_settings_getstr_default(settings, "audio.driver");
}

int fluidsynth_init()
{ 
  g_debug("\nStarting FLUIDSYNTH\n"); 
  /* Create the settings. */
  if (!settings)
    start_fluid_settings(); 

  /* Change the settings if necessary*/
  fluid_settings_setstr(settings, "audio.driver", Denemo.prefs.fluidsynth_audio_driver->str);
  fluid_settings_setstr(settings, "synth.reverb.active" , Denemo.prefs.fluidsynth_reverb == TRUE ? "yes":"no");
  fluid_settings_setstr(settings, "synth.chorus.active" , Denemo.prefs.fluidsynth_chorus == TRUE ? "yes":"no");
  
  /* Create the synthesizer. */
  synth = new_fluid_synth(settings);
  if (!synth){
    g_warning("\nFailed to create the settings\n");
    //fluidsynth_shutdown();
    delete_fluid_synth(synth);
    return 1;
  }

  /* Create the audio driver. */
  adriver = new_fluid_audio_driver(settings, synth);
  if (!adriver){
    g_warning("\nFailed to create the audio driver\n");
    fluidsynth_shutdown();
    return 1;
  }
  
  /* Load a SoundFont*/
gint  sfont_id = fluid_synth_sfload(synth, Denemo.prefs.fluidsynth_soundfont->str, 0);
  if (sfont_id == -1){
    g_warning("\nFailed to load the soundfont\n");
    fluidsynth_shutdown();
    return 1;
  }
  
  /* Select bank 0 and preset 0 in the SoundFont we just loaded on
  channel 0 */
  fluid_synth_program_select(synth, 0, sfont_id, 0, 0);

   return 0;
}                                                                                                                              
                    
void fluidsynth_shutdown()
{
  /* Clean up */
  g_debug("\nStopping FLUIDSYNTH\n");
  if (adriver)
    delete_fluid_audio_driver(adriver);
  adriver = NULL;
  if (synth)
    delete_fluid_synth(synth);
  synth = NULL;
  if (settings)
    delete_fluid_settings(settings); 
  settings = NULL;
}

void
fluidsynth_start_restart (void){
  if (synth){
    fluidsynth_shutdown();
    fluidsynth_init();
  }
  else {
    fluidsynth_init();
  }
}

static gboolean timer_callback(gpointer notenum){
  gint key = (gint) notenum;
  if (synth)
    fluid_synth_noteoff(synth, 0, key);
  return FALSE;
}

void fluid_playpitch(int key, int duration)
{
  /* Play a note */
  if (synth){
    fluid_synth_noteon(synth, 0, key, 80);
    g_timeout_add(duration, timer_callback, (gpointer) key); 
  }
}

void fluid_output_midi_event(unsigned char *buffer)
{
  if (synth){
    if ((buffer[0] & SYS_EXCLUSIVE_MESSAGE1) == NOTE_ON)
      fluid_synth_noteon(synth, buffer[0] & 0x0f, buffer[1], 80);
    if ((buffer[0] & SYS_EXCLUSIVE_MESSAGE1) == NOTE_OFF) 
      fluid_synth_noteoff(synth, buffer[0] & 0x0f, buffer[1]);
  }
}

/**
 * Select the soundfont to use for playback
 */
void
choose_sound_font (GtkWidget * widget, GtkWidget *fluidsynth_soundfont)
{
  GtkWidget *sf;
  GtkFileFilter *filter;

  sf = gtk_file_chooser_dialog_new (_("Choose SoundFont File"),
				     GTK_WINDOW (Denemo.window),
				    GTK_FILE_CHOOSER_ACTION_OPEN,
				    GTK_STOCK_CANCEL,
				    GTK_RESPONSE_REJECT,
				    GTK_STOCK_OPEN,
				    GTK_RESPONSE_ACCEPT, NULL);

  //TODO Should we filter????
  //filter = gtk_file_filter_new ();
  //gtk_file_filter_set_name (filter, "Soundfont file");
  //gtk_file_filter_add_pattern (filter, "*.sf");
  //gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (fs), filter);

  gtk_widget_show_all (sf);
  if (gtk_dialog_run (GTK_DIALOG (sf)) == GTK_RESPONSE_ACCEPT)
    {
      g_string_assign (Denemo.prefs.fluidsynth_soundfont,
		       gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (sf)));
  	/* this will only work for 1 sound font */
        gtk_entry_set_text (GTK_ENTRY (fluidsynth_soundfont), Denemo.prefs.fluidsynth_soundfont->str);

    }
  gtk_widget_destroy (sf);
}

gboolean fluidsynth_read_smf_events()
{

  smf_event_t *event = Denemo.gui->si->smf?smf_peek_next_event(Denemo.gui->si->smf):NULL;
  
  if (!synth)
    return FALSE;

  if (!playing_piece)
    return FALSE;

  if (event == NULL || event->time_seconds>end_time){ 
    playing_piece = FALSE;
    return FALSE;
  }
  else 
    playing_piece = TRUE;
  
   /* Skip over metadata events. */
  if (smf_event_is_metadata(event)) {
    event = smf_get_next_event(Denemo.gui->si->smf);
    return TRUE; 
  } 
     
  if ((get_time() - start_player) >= event->time_seconds){
     event = smf_get_next_event(Denemo.gui->si->smf);

 
#if 0
	 /* Temporary fix */
    /*this should go in a function that assigns after staff options or preferences */
    //fluid_synth_program_change(synth, 0, 5);

    if ((event->midi_buffer[0] & SYS_EXCLUSIVE_MESSAGE1) == NOTE_ON) 
      fluid_synth_noteon(synth, 0 /*(event->midi_buffer[0] & 0x0f)*/, event->midi_buffer[1], event->midi_buffer[2]);
    if ((event->midi_buffer[0] & SYS_EXCLUSIVE_MESSAGE1) == NOTE_OFF)
      fluid_synth_noteoff(synth, 0 /*(event->midi_buffer[0] & 0x0f)*/, event->midi_buffer[1]);

#else
    gint chan = (event->midi_buffer[0] & 0x0f);
    //g_print("message %x %x\n", event->midi_buffer[0] & SYS_EXCLUSIVE_MESSAGE1, PROGRAM_CHANGE);
    int success;
    switch((event->midi_buffer[0] & SYS_EXCLUSIVE_MESSAGE1))
 {
       case NOTE_ON:
         success = fluid_synth_noteon(synth, chan,  event->midi_buffer[1], event->midi_buffer[2]);
	 //g_print("success = %d\n", success);
	 break;
       case NOTE_OFF:
         fluid_synth_noteoff(synth, chan,  event->midi_buffer[1]);
	 break; 
       case CONTROL_CHANGE:
         return fluid_synth_cc(synth, chan, event->midi_buffer[1], event->midi_buffer[2]);
	 break; 
                             
       case PROGRAM_CHANGE:
	 g_print("changing on chan %d to prog? %d\n", chan,  event->midi_buffer[1]);
         success = fluid_synth_program_change(synth, chan,  event->midi_buffer[1]);
	 // g_print("success = %d\n", success);
	 break;
 
	 //     case CHANNEL_PRESSURE:
	 //return fluid_synth_channel_pressure(synth, chan,  event->midi_buffer[1]);
 
       case PITCH_BEND:
         fluid_synth_pitch_bend(synth, chan, event->midi_buffer[1] + (event->midi_buffer[2]<<8)
				       /*I think! fluid_midi_event_get_pitch(event)*/);
	 break;
 
       case MIDI_SYSTEM_RESET:
         fluid_synth_system_reset(synth);
	 break;
 }

#endif
  }
  
  return TRUE;
}

static gint move_on(DenemoGUI *gui){
  if(timeout_id==0 || playing_piece==FALSE)
    return FALSE;
  set_currentmeasurenum (gui, gui->si->currentmeasurenum+1);
  return TRUE;
}

gint fluid_kill_timer(void){
  g_debug("fluidsynth kill timer %d\n", timeout_id);
  if(timeout_id>0)
    g_source_remove(timeout_id);
  timeout_id = 0;
  if(kill_id)
    g_source_remove(kill_id);
  kill_id = 0;
  return 1;
}


void fluid_midi_play(void)
{
  DenemoGUI *gui = Denemo.gui;
  start_player = get_time();
  playing_piece = TRUE;
  
  if (!synth)
    return;

  if((gui->si->smf==NULL) || (gui->si->smfsync!=gui->si->changecount))
    exportmidi (NULL, gui->si, 1, 0/* means to end */);
  if (Denemo.gui->si->smf == NULL) {
    g_critical("Loading SMF failed.");
  } else {
    smf_rewind(Denemo.gui->si->smf);
    g_idle_add(fluidsynth_read_smf_events, NULL);
  }
#if 1
  playback_duration = smf_get_length_seconds(Denemo.gui->si->smf);
  
  /* TODO make the below some sort of function this is copy  
     pasted section from jackmidi.c which was copied from 
     somewhere else also
   */
  DenemoObject *curobj;
  start_time = 0.0;
  curobj = get_mark_object();
  if(curobj==NULL && gui->si->currentobject)
    curobj = gui->si->currentobject->data;
  if(curobj && curobj->midi_events) {
    smf_event_t *event = curobj->midi_events->data;
    start_time = event->time_seconds;
    g_debug("\nsetting start %f\n", start_time);
  }
  end_time = playback_duration;
  curobj = NULL;
  curobj =  get_point_object();
  if(curobj && curobj->midi_events)/*is this ever true?*/ { 
    smf_event_t *event = g_list_last(curobj->midi_events)->data;
    end_time = event->time_seconds;
    g_debug("\nsetting end %f\n", end_time);	   
  } 


  if(start_time>end_time) {
    gdouble temp = start_time;
    start_time = end_time;
    end_time = temp;
  }
  playback_duration = end_time - start_time;
  g_debug("\nstart %f for %f seconds\n",start_time, playback_duration);

    if(gui->si->end==0) {//0 means not set, we move the cursor on unless the specific range was specified
      DenemoStaff *staff = (DenemoStaff *) gui->si->currentstaff->data;
      //FIXME add a delay before starting the timer.
      timeout_id = g_timeout_add ( 4*((double)staff->timesig.time1/(double)staff->timesig.time2)/(gui->si->tempo/(60.0*1000.0)), (GSourceFunc)move_on, gui);
      // g_print("Setting end time to %f %u\n", duration*1000, (guint)(duration*1000));
      kill_id = g_timeout_add ((guint)(playback_duration*1000), (GSourceFunc)fluid_kill_timer, NULL);
    }
#endif  
  
}

void
fluid_midi_stop(void)
{
  playing_piece = FALSE;  
}

/* give audible feedback for entering a rhythmic element */
void
fluid_rhythm_feedback(gint duration, gboolean rest, gboolean dot) {
  fluid_playpitch(64+(1+duration), 200);
  g_print("playing %d %d\n", 64+(1+duration), (60/(4*Denemo.gui->si->tempo*(1<<duration)))*1000);

}



#else // _HAVE_FLUIDSYNTH_
void fluid_playpitch(int key, int duration){}
void fluid_output_midi_event(unsigned char *buffer){}
void fluid_midi_play(void){}
void fluid_midi_stop(void){}
int fluid_kill_timer(void){}
#endif 

