/* print.cpp
 * 
 * basic printing support for GNU Denemo
 * outputs to a dvi file
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 2001-2005 Adam Tee
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_WAIT_H
#include <wait.h>
#endif

#include <errno.h>

#include "print.h"
#include "prefops.h"
#include "exportlilypond.h"
#include "utils.h"


/* truncate epoint after 20 lines replacing the last three chars in that case with dots */
static void truncate_lines(gchar *epoint) {
  gint i;
  for(i=0;i<20 && *epoint;i++) {
    while (*epoint && *epoint!='\n')
      epoint++;
    if(*epoint)
      epoint++;
  }
  if(epoint)
    *epoint-- = '\0';
  /* replace last three chars with ... This is always possible if epoint is not NULL */
  if(*epoint)
    for(i=3;i>0;i--)
      *epoint-- = '.';
}
/***
 * Run the command line convert-ly to get the lilypond output 
 * current with the version running on the users computer
 *
 */

void convert_ly(gchar *lilyfile){
  GError *err = NULL;
#ifdef G_OS_WIN32
/*   gchar *dirname = g_path_get_dirname (Denemo.prefs.lilypath->str ); */
/*   gchar *convert = g_build_filename(dirname, "convert-ly.py");// FIXME memory leaks */

#else
/** This convert stuff could be made a seperate function **/
  gchar *convert = "convert-ly";

  gchar *conv_argv[] = {
    convert,
    "-e",
    lilyfile,
    NULL
  };

  g_spawn_sync (locatedotdenemo (),		/* dir */
		conv_argv, NULL,	/* env */
		G_SPAWN_SEARCH_PATH, NULL,	/* child setup func */
		NULL,		/* user data */
		NULL,		/* stdout */
		NULL,		/* stderr */
		NULL, &err);

  if (err != NULL)
    {
      warningdialog("Could not execute lilypond's convert-ly program - check lilypond installation or just ignore");
      g_warning ("%s", err->message);
      g_error_free (err);
      err = NULL;
    }
#endif
}
 
/* Run the LilyPond interpreter on the file (filename).ly
 * putting the PDF output in (filename).pdf
 * start an external PDF viewer on that file.
 * parse first LilyPond error and position the cursor in gui->textview on it
 */
void
run_lilypond_and_viewer(gchar *filename, DenemoGUI *gui) {
  GError *err = NULL;
  gchar *printfile;
  if (gui->lilycontrol.excerpt == TRUE)
	printfile = g_strconcat (filename, ".png", NULL);
  else
  	printfile = g_strconcat (filename, ".pdf", NULL);
  
  FILE *fp = fopen(printfile, "w");
  if(fp)
    fclose(fp);
  else {
    //FIXME use filename in message
    warningdialog("Could not open ~/.denemo/denemoprint.pdf, check permissions");
    return;
  }
  gchar *lilyfile = g_strconcat (filename, ".ly", NULL);
  convert_ly(lilyfile);
  //pointer to pointer that changes according to *argv[]

  gchar **arguments;
  if (gui->lilycontrol.excerpt == TRUE){
	  gchar *argv[] = {
		    Denemo.prefs.lilypath->str,
		    "--png",
		    "-b",
		    "eps", 
		    "-o",
		    filename,
		    lilyfile,
		    NULL
	  };
	  arguments = argv;
  }
  else {

	  gchar *argv[] = {
	       	    Denemo.prefs.lilypath->str,
		    "--pdf",
		    "-o",
		    filename,
		    lilyfile,
		    NULL
	  };
	  arguments = argv;
  }
    
  gchar *output=NULL, *errors=NULL;
  g_spawn_sync (locatedotdenemo (),		/* dir */
		arguments, NULL,	/* env */
		G_SPAWN_SEARCH_PATH, NULL,	/* child setup func */
		NULL,		/* user data */
		&output,		/* stdout */
		&errors,		/* stderr */
		NULL, &err);
  gchar *filename_colon = g_strdup_printf("%s%s",lilyfile,":");
  //g_print("filename_colon = %s\n", filename_colon);
  gchar *epoint = NULL;
  if(errors) 
    epoint = g_strstr_len (errors,strlen(errors), filename_colon);

  if(epoint) {
    gint line, column;
    gint cnv = sscanf(epoint+strlen(filename_colon), "%d:%d", &line, &column);
    truncate_lines(epoint);/* truncate epoint if it has too many lines */
    if(cnv==2) {
      line--;/* make this 0 based */
      if(line >= gtk_text_buffer_get_line_count(gui->textbuffer))
	warningdialog("Spurious line number"), line = 0;
      /* gchar *errmsg = g_strdup_printf("Error at line %d column %d %d", line,column, cnv); */
      /*     warningdialog(errmsg); */
      infodialog(epoint);
      if(gui->textbuffer) {
	set_lily_error(line+1, column, gui);
      } 
    }
    else {
      set_lily_error(0, 0, gui);
      warningdialog(epoint);
    }
  } else
    set_lily_error(0, 0, gui);/* line 0 meaning no line */
  highlight_lily_error(gui);
  g_free(filename_colon);
  if (err != NULL)
    {
      if(errors)
	infodialog(errors);
      warningdialog("Could not execute lilypond - check Edit->preferences->externals->lilypond setting\nand lilypond installation");
      g_warning ("%s", err->message);
      g_error_free (err);
      err = NULL;
    }


  if((fp=fopen(printfile, "r"))) {
    if(getc(fp)==EOF) {
      g_warning ("Failed to read %s", (gchar *) printfile);
      warningdialog("Cannot make score, probably errors in lilypond output");
      fclose(fp);
      return;
    }
  } else
  {
    g_warning ("Failed to find %s", (gchar *) printfile);
    warningdialog("Could not create a pdf - check permissions");
    return;
  }
    
  //gchar **arguments;
  //g_print("using %s\n", printfile);
  if (gui->lilycontrol.excerpt == TRUE){
  	  gchar *args[] = {
	    Denemo.prefs.imageviewer->str,
	    printfile,
	    NULL
	  };
	  arguments = args;
  }
  else {
	  gchar *args[] = {
	    Denemo.prefs.pdfviewer->str,
	    printfile,
	    NULL
	  };
	  arguments = args;  
  }


  GPid printpid;//ignored
  g_spawn_async (locatedotdenemo (),		/* dir */
		 arguments, NULL,	/* env */
		 G_SPAWN_SEARCH_PATH, /* search in path for executable */
		 NULL,	/* child setup func */
		 NULL,		/* user data */		
		 NULL, /* FIXME &printpid see g_spawn_close_pid(&printpid) */
		 &err);
  

  if (err != NULL)
    {
      g_warning ("Failed to find %s", Denemo.prefs.pdfviewer->str);
      warningdialog("Cannot display: Check Edit->Preferences->externals\nfor your PDF viewer");
      g_warning ("%s", err->message);
      g_error_free (err);
      err = NULL;
    }

  g_free(printfile);
}

/* returns the base name (~/.denemo/denemoprint usually) used as a base
   filepath for printing. On windows there is some filelocking trouble.
   The returned string should not be freed.
*/
   
gchar *get_printfile_pathbasename(void) {
  static gchar *filename = NULL;
#ifdef G_OS_WIN32
 {
   static int count=1;
   gchar *denemoprint = g_strdup_printf("denemoprint%d", count);
   count++;
   if(filename)
     g_free(filename);
   filename = g_build_filename ( locatedotdenemo (), denemoprint, NULL);
   g_free(denemoprint);
 }
#else
  if (!filename)
      filename = g_build_filename ( locatedotdenemo (), "denemoprint", NULL);
#endif
  return filename;
}

/*  Print function 
 *  Save file in lilypond format
 *  Fork and run lilypond
 *  TODO Add in lpr command
 */
static void
print (DenemoGUI * gui, gboolean part_only, gboolean all_movements)
{
  DenemoScore *si = gui->si;
  gchar *filename = get_printfile_pathbasename();
  gchar *lilyfile = g_strconcat (filename, ".ly", NULL);
  remove (lilyfile);
  if(part_only)
    export_lilypond_part (lilyfile, gui, all_movements);
  else
    exportlilypond (lilyfile, gui,  all_movements);
  run_lilypond_and_viewer(filename, gui);
  g_free(lilyfile);
}


/** 
 * Dialog function used to select measure range 
 *
 */

void
printrangedialog(DenemoGUI * gui){
  GtkWidget *dialog;
  GtkWidget *label;
  GtkWidget *hbox;
  GtkWidget *from_measure;
  GtkWidget *to_measure;
  
  dialog = gtk_dialog_new_with_buttons (_("Print Excerpt Range"),
	 GTK_WINDOW (gui->window),
	 (GtkDialogFlags) (GTK_DIALOG_MODAL |
	      GTK_DIALOG_DESTROY_WITH_PARENT),
	 GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
	 GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

  hbox = gtk_hbox_new (FALSE, 8);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox, TRUE, TRUE, 0);

  gint max_measure =
  g_list_length (((DenemoStaff *) (gui->si->thescore->data))->measures);

  label = gtk_label_new (_("Print from Measure"));
  gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
 
  from_measure =
  gtk_spin_button_new_with_range (1.0, (gdouble) max_measure, 1.0);
  gtk_box_pack_start (GTK_BOX (hbox), from_measure, TRUE, TRUE, 0);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (from_measure),
			     (gdouble) gui->si->firstmeasuremarked);

  label = gtk_label_new (_("to"));
  gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

  to_measure =
  gtk_spin_button_new_with_range (1.0, (gdouble) max_measure, 1.0);
  gtk_box_pack_start (GTK_BOX (hbox), to_measure, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), to_measure, TRUE, TRUE, 0);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (to_measure),
			     (gdouble) gui->si->lastmeasuremarked);

  gtk_widget_show (hbox);
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_MOUSE);
  gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
  gtk_widget_show_all (dialog);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      gui->si->firstmeasuremarked =
	gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (from_measure));
      gui->si->lastmeasuremarked =
	gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (to_measure));
      gtk_widget_destroy (dialog);
    }
  else 
    {
      gui->si->firstmeasuremarked = gui->si->lastmeasuremarked = 0;
    }
  if(gui->si->firstmeasuremarked) {
    gui->si->markstaffnum = gui->si->firststaffmarked = 1;
    gui->si->laststaffmarked = g_list_length(gui->si->thescore);
  }
  gtk_widget_destroy (dialog);
}


/* callback to print whole of score */
void
printall_cb (GtkAction * action, DenemoGUI * gui) {
  gchar *str = g_strdup_printf("No direct printing yet\nWe will run the PDF viewer program %s so you can use its print command.\nYou can change the PDF viewer using \nEdit->Preferences->Externals->Pdf viewer.", Denemo.prefs.pdfviewer->str);
  warningdialog(str);
  g_free(str);
  print(gui, FALSE, TRUE);
}
/* callback to print current part (staff) of score */
void
printpart_cb (GtkAction * action, DenemoGUI * gui) {
  if(gui->si->markstaffnum)
    if(confirm("A range of music is selected","Print whole file?")){
      gui->si->markstaffnum=0;
    }
  if((gui->movements && g_list_length(gui->movements)>1) && 
     (confirm("This piece has several movements", "Print this part from all of them?")))
    print(gui, TRUE, TRUE);
  else
   print(gui, TRUE, FALSE);
  
}
void
printpreview_cb (GtkAction * action, DenemoGUI * gui) {
  if(gui->si->markstaffnum)
    if(confirm("A range of music is selected","Print whole file?")){
      gui->si->markstaffnum=0;
    }
  print(gui, FALSE, TRUE);
}
void
printexcerptpreview_cb (GtkAction * action, DenemoGUI * gui) {
  gui->lilycontrol.excerpt = TRUE;
  if(gui->si->markstaffnum) {
  
  } else {
    printrangedialog(gui);

  }
  print(gui, FALSE, FALSE);
  gui->lilycontrol.excerpt = FALSE;

}

/**
 * Export pdf callback prompts for filename
 *
 */

void
export_pdf_action (GtkAction * action, DenemoGUI * gui)
{

  GtkWidget *file_selection;

  file_selection = gtk_file_chooser_dialog_new (_("Export PDF"),
						GTK_WINDOW (gui->window),
						GTK_FILE_CHOOSER_ACTION_SAVE,
						GTK_STOCK_CANCEL,
						GTK_RESPONSE_REJECT,
						GTK_STOCK_SAVE,
						GTK_RESPONSE_ACCEPT, NULL);

  gtk_widget_show_all (file_selection);
  gboolean close = FALSE;
  do
    {

      if (gtk_dialog_run (GTK_DIALOG (file_selection)) == GTK_RESPONSE_ACCEPT)
	{
	  gchar *filename =
	    gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_selection));

	  if (replace_existing_file_dialog
	      (filename, GTK_WINDOW (gui->window), -1))
	    {
	      export_pdf (filename, gui);
	      close = TRUE;
	    }
	  g_free (filename);
	}
      else
	{
	  close = TRUE;
	}

    }
  while (!close);

  gtk_widget_destroy (file_selection);
}

/**
 * Does all the export pdf work.
 * calls exportmudela and then  
 * runs lilypond to a create a temporary pdf file and 
 * renames to filename.pdf
 *
 *	@param filename filename to save score to
 *  @param gui pointer to the DenemoGUI structure
 */
void
export_pdf (const gchar * filename, DenemoGUI * gui)
{
  const gchar *tmpdir = locatedotdenemo ();
  gchar *tmpfile;
  gchar *mudelafile;
  gchar *midifile;
  gchar *dvifile;
  gchar *psfile;
  gchar *pdffile;
  GError *err = NULL;
  gint exit_status;
  gboolean ok;

  /* look for lilypond */
  gchar *lilypath = g_find_program_in_path (Denemo.prefs.lilypath->str);
  /*** this can replaced with a utility ****/
  if (lilypath == NULL)
    {
      /* show a warning dialog */
      GtkWidget *dialog =
        gtk_message_dialog_new (GTK_WINDOW (gui->window),
                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                GTK_MESSAGE_WARNING,
                                GTK_BUTTONS_OK,
                                _("Could not find %s"),
                                Denemo.prefs.lilypath->str);
      gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
                                                _("Please edit lilypond path "
                                                  "in the preferences."));
      gtk_dialog_run (GTK_DIALOG (dialog));

      /* free the memory and return */
      gtk_widget_destroy (dialog);
      return;
    }

  /* create a temp (and not existing) filepath in .denemo folder */
  do
    {
      tmpfile = get_temp_filename (NULL);
      mudelafile = g_strconcat (tmpfile, ".ly", NULL);
      midifile = g_strconcat (tmpfile, ".midi", NULL);
      dvifile = g_strconcat (tmpfile, ".dvi", NULL);
      psfile = g_strconcat (tmpfile, ".ps", NULL);
      pdffile = g_strconcat (tmpfile, ".pdf", NULL);

      if (g_file_test (mudelafile, G_FILE_TEST_EXISTS) ||
          g_file_test (midifile, G_FILE_TEST_EXISTS) ||
          g_file_test (dvifile, G_FILE_TEST_EXISTS) ||
          g_file_test (psfile, G_FILE_TEST_EXISTS) ||
          g_file_test (pdffile, G_FILE_TEST_EXISTS))
        ok = FALSE;
      else
        ok = TRUE;

      if (!ok)
        {
          g_free (tmpfile);
          g_free (mudelafile);
          g_free (midifile);
          g_free (dvifile);
          g_free (psfile);
          g_free (pdffile);
        }
    }
  while(!ok);

  /* generate the lilypond file */
  exportlilypond (mudelafile, gui, TRUE);

  /* generate the pdf file */
  gchar *argv[] =
    {
      Denemo.prefs.lilypath->str,
      "--pdf",
      "-o",
      tmpfile,
      mudelafile,
      NULL
    };

  gchar *output=NULL, *errors=NULL;
  g_spawn_sync (locatedotdenemo (),		/* dir */
		argv, NULL,	/* env */
		G_SPAWN_SEARCH_PATH, NULL,	/* child setup func */
		NULL,		/* user data */
		&output,		/* stdout */
		&errors,		/* stderr */
		NULL, &err);

  if (err != NULL)
    {
      g_warning ("%s", err->message);
      g_error_free (err);
      remove (mudelafile);

      g_free (tmpfile);
      g_free (mudelafile);
      g_free (midifile);
      g_free (dvifile);
      g_free (psfile);
      g_free (pdffile);

      return;
    }

  /* move the pdf file to its destination */
  if (rename (pdffile, filename) != 0)
    g_warning ("Failed to rename %s to %s\n", pdffile, filename);

  /* remove unnecessary files and free the memory */
  remove (mudelafile);
  remove (midifile);
  remove (dvifile);
  remove (psfile);

  g_free (tmpfile);
  g_free (mudelafile);
  g_free (midifile);
  g_free (dvifile);
  g_free (psfile);
  g_free (pdffile);
}
