//      sourceaudio.c
//      
//      Copyright 2013 Richard Shann <rshann@virgin.net>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 3 of the License, or
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


#include <stdio.h>
#include <sndfile.h>
#include <fcntl.h>
#include <string.h>
#include "audiofile.h"
#include "file.h"
#include "prefops.h"
#include "utils.h"

const gchar *
recorded_audio_filename (void)
{
  static gchar *filename;
  if (filename == NULL)
    filename = g_build_filename (locatedotdenemo (), "denemo_audio.1channel-floats", NULL);
  return filename;
}

gboolean
export_recorded_audio ()
{
  const gchar *filename = recorded_audio_filename ();
  SF_INFO out;
  if (filename)
    {
      gsize length;
      float *data;
      if (g_file_get_contents (filename, (gchar **)&data, &length, NULL))
        {
          gchar *outfile = file_dialog ("Give output audio file name, with .ogg or .wav extension", FALSE, Denemo.prefs.denemopath->str);
          if (outfile)
            {
              gint len = strlen (outfile);
              if (len > 4)
                {
                  if (*(outfile + len - 3) == 'o')
                    {
                      out.format = SF_FORMAT_VORBIS | SF_FORMAT_OGG;
                    }
                  else
                    {
                      out.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
                    }
                  out.channels = 1;
                  out.samplerate = 44100;
                  gpointer outsnd = sf_open (outfile, SFM_WRITE, &out);
                  if (outsnd)
                    {
                      sf_write_float (outsnd, data, length/sizeof(float));
                      g_free(data);
                      sf_close (outsnd);
                      return TRUE;
                    }
                  else
                    {
                      g_warning ("Unable to open file %s for writing this format\n", outfile);
                    }
                  g_free (outfile);
                }
            }
          
        }
        else
        {
          if (Denemo.prefs.maxrecordingtime)
            warningdialog (_("No audio recording has been made.\nSee Playback Controls - Record Button"));
          else
            warningdialog (_("The preference set for recording time is 0 - nothing is recorded.\nSee Edit->Change Preferences Audio/Midi Tab"));
        }
    }
  return FALSE;
}