/* texteditors.c
 * text editors for editing scripts, adding comments etc
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 2009  Richard Shann
 * 
 */

#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcestyleschememanager.h>
#include <gtksourceview/gtksourceprintcompositor.h>
#if GTK_MAJOR_VERSION==3
#include <gdk/gdkkeysyms-compat.h>      //FIXME Look for something more gtk3 like
#else
#include <gtksourceview/gtksourceiter.h>
#endif
#include <gtksourceview/gtksourcebuffer.h>
#include "ui/texteditors.h"
#include "core/view.h"
#include "scripting/scheme-callbacks.h"

static void find_cb (GtkAction * action, gpointer user_data);

static void replace_cb (GtkAction * action, gpointer user_data);

/* returns newly allocated string containing current Scheme in the script_view
 caller must free
*/
gchar *
get_script_view_text (void)
{
  GtkTextIter startiter, enditer;
  GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (Denemo.script_view));
  gtk_text_buffer_get_start_iter (buffer, &startiter);
  gtk_text_buffer_get_end_iter (buffer, &enditer);
  return gtk_text_buffer_get_text (buffer, &startiter, &enditer, FALSE);
}

/* execute the script that is in the Scheme script window */
void
executeScript (void)
{
  gchar *text = Denemo.project->script;
  g_debug ("Calling script %s\n", text);
  stage_undo (Denemo.project->movement, ACTION_STAGE_END);        //undo is a queue so this is the end :)
  (void) call_out_to_guile (text);
  stage_undo (Denemo.project->movement, ACTION_STAGE_START);
}


/* execute the line of scheme script that is in the Scheme CLI */
void
executeCLI (GtkWidget * button, GtkEntry * entry)
{
  gchar *display = NULL;
  if (entry)
    {
      display = g_strdup_printf ("%s%s%s", "(format #t \"~%=> ~A~%\"", gtk_entry_get_text (entry), ")\n");
      //g_debug("displaying %s\n", display);
      (void) call_out_to_guile (display);
      g_free (display);
    }
  else
    g_critical ("entry is NULL!!!!");
}

/* Return number of characters in Scheme script */
//TODO: Avoid to use gtk widgets for this since it can be used in non 
//interactive mode.
gint
getNumCharsSchemeText (void)
{
  GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (Denemo.script_view));
  return gtk_text_buffer_get_char_count (buffer);
}

void
deleteSchemeText (void)
{
  if(!Denemo.non_interactive){
    GtkTextIter startiter, enditer;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (Denemo.script_view));
    gtk_text_buffer_get_start_iter (buffer, &startiter);
    gtk_text_buffer_get_end_iter (buffer, &enditer);
    gtk_text_buffer_delete (buffer, &startiter, &enditer);
  }
  else
  {
    g_free(Denemo.project->script);
    Denemo.project->script = NULL;
  }
}

void
appendSchemeText (gchar * text)
{  
  if(!Denemo.non_interactive){
    GtkTextIter enditer;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer ((GtkTextView *) (Denemo.script_view));
    gtk_text_buffer_get_end_iter (buffer, &enditer);
    gtk_text_buffer_insert (buffer, &enditer, text, -1);
  }

  else{
    if(Denemo.project->script){
      gchar* old_script = Denemo.project->script;
      Denemo.project->script = g_strconcat(old_script, text, NULL);
      g_free(old_script);
    }
    else
      Denemo.project->script = text;
  }
}

static gint
hide_scheme (GtkAction * action, GdkEvent * event, GtkWidget * w)
{
  
  if(Denemo.ScriptRecording) {
        gtk_action_activate (gtk_action_group_get_action (Denemo.action_group, RecordScript_STRING));
        infodialog(_("Turning off Recording scheme"));
    }
  activate_action ("/MainMenu/ViewMenu/ToggleScript");
  return TRUE;
}


static void
save_scheme_text_as (GtkWidget * widget, GtkWidget * textview)
{
  gchar **pfilename = g_object_get_data (G_OBJECT (textview), "pfilename");
  gchar *text = get_script_view_text ();
  GtkWidget *label;
  GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (Denemo.script_view));
  GtkWidget *dialog = gtk_file_chooser_dialog_new (_("Save Scheme Text as..."),
                                                   NULL /*GTK_WINDOW(gtk_text_view_get_window(GTK_TEXT_VIEW(Denemo.script_view), GTK_TEXT_WINDOW_WIDGET)) */ ,
                                                   GTK_FILE_CHOOSER_ACTION_SAVE,
                                                   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                   GTK_STOCK_SAVE, GTK_RESPONSE_OK, NULL);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
    {
      g_free (*pfilename);
      *pfilename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

      if (g_file_test (*pfilename, G_FILE_TEST_EXISTS))
        {
          gtk_widget_destroy (dialog);
          dialog = gtk_dialog_new_with_buttons (_("File already exists"),  //FIXME I think there is a function to do this already.
                                                NULL /*GTK_WINDOW(gtk_text_view_get_window(GTK_TEXT_VIEW(Denemo.script_view), GTK_TEXT_WINDOW_WIDGET)) */ ,
                                                GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
          gchar* labeltext = g_strdup_printf(_("The file %s already exists.\nDo you want to overwrite it?"), *pfilename);
          label = gtk_label_new (labeltext);
          g_free (labeltext);
          GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
          gtk_container_add (GTK_CONTAINER (content_area), label);
          gtk_widget_show_all (dialog);

          if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_CANCEL)
            {
              g_free (text);
              gtk_widget_destroy (dialog);
              return;
            }
        }
      g_file_set_contents (*pfilename, text, -1, NULL);
      gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (buffer), FALSE);
    }
  g_free (text);
  gtk_widget_destroy (dialog);
}


static void
save_scheme_text (GtkWidget * widget, GtkWidget * textview)
{
  gchar **pfilename = g_object_get_data (G_OBJECT (textview), "pfilename");
  GtkTextBuffer *buffer;
  if (*pfilename == NULL)
    save_scheme_text_as (NULL, textview);
  else
    {
      gchar *text = get_script_view_text ();
      buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (Denemo.script_view));
      g_file_set_contents (*pfilename, text, -1, NULL);
      gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (buffer), FALSE);
      g_free (text);
    }
}

gboolean
save_scheme_dialog (GtkTextBuffer * buffer, GtkWidget * textview)
{
  gchar **pfilename = g_object_get_data (G_OBJECT (textview), "pfilename");
  GtkWidget *dialog;
  GtkWidget *label;
  gint response;
  GtkWidget *content_area;

  if (gtk_text_buffer_get_modified (buffer))
    {
      dialog = gtk_dialog_new_with_buttons (_("Scheme text changed"), NULL /*GTK_WINDOW(gtk_text_view_get_window(GTK_TEXT_VIEW(Denemo.script_view), GTK_TEXT_WINDOW_WIDGET)) */ ,
                                            GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_YES, GTK_RESPONSE_YES, GTK_STOCK_NO, GTK_RESPONSE_NO, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);


      if (*pfilename == NULL)
        label = gtk_label_new (_("\nDo you want to save the changes in a new file?\n\n"));
      else
        {
          gchar* labeltext = g_strdup_printf(_("\nDo you want to save the changes in %s ?\n\n"), *pfilename);
          label = gtk_label_new (labeltext);
          g_free (labeltext);
        }

      content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
      gtk_container_add (GTK_CONTAINER (content_area), label);

      gtk_widget_show_all (dialog);

      response = gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      if (response == GTK_RESPONSE_YES)
        save_scheme_text (NULL, textview);
      else if (response == GTK_RESPONSE_CANCEL)
        return FALSE;

    }
  return TRUE;
}



static void
load_scheme_from_file (GtkWidget * widget, GtkWidget * textview)
{
  gchar **pfilename = g_object_get_data (G_OBJECT (textview), "pfilename");
  gchar *text = NULL;
  GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (Denemo.script_view));
  GtkWidget *dialog = gtk_file_chooser_dialog_new (_("Open File"),
                                                   NULL /*GTK_WINDOW(gtk_text_view_get_window(GTK_TEXT_VIEW(Denemo.script_view),GTK_TEXT_WINDOW_WIDGET)) */ ,
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                   GTK_STOCK_OPEN, GTK_RESPONSE_OK, NULL);

  if (!save_scheme_dialog (buffer, textview))
    {
      return;
    }


  if (gtk_dialog_run ((GTK_DIALOG (dialog))) == GTK_RESPONSE_OK)
    {
      if (*pfilename)
        g_free (*pfilename);
      *pfilename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      g_file_get_contents (*pfilename, &text, NULL, NULL);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (buffer), text, -1);
      gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (buffer), FALSE);
    }

  g_free (text);
  gtk_widget_destroy (dialog);

}

void
clear_scheme_window (GtkWidget * widget, GtkWidget * textview)
{
  gchar **pfilename = g_object_get_data (G_OBJECT (textview), "pfilename");
  GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (Denemo.script_view));
  if (!save_scheme_dialog (buffer, textview))
    return;

  g_free (*pfilename);
  *pfilename = NULL;
  gtk_text_buffer_set_text (GTK_TEXT_BUFFER (buffer), "", 0);
  gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (buffer), FALSE);
}


static gboolean
keypress (GtkEntry * w, GdkEventKey * event)
{
  if (event->keyval == GDK_Return)
    executeCLI (NULL, w);
  //let the normal handler have the keypress
  return FALSE;
}

static void
scheme_changed_cb (GtkSourceBuffer *buffer){
  if(Denemo.project->script)
    g_free(Denemo.project->script);
  Denemo.project->script = get_script_view_text();
}

/*
 create_editor_window()
 create a text window for editing
*/

static GtkWidget *
create_editor_window (void)
{
  GtkWidget *TextView;
  GtkSourceBuffer *buffer;
  GtkSourceLanguageManager *LanguageManager = gtk_source_language_manager_get_default ();
  //GtkWidget *w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  GtkWidget *menu, *menuBar, *fileMenu, *item;
  gchar **filename = g_malloc0 (sizeof (gchar *));
  GtkSourceLanguage* language = NULL;


  buffer = gtk_source_buffer_new (NULL);
  g_signal_connect (G_OBJECT (buffer), "changed", G_CALLBACK (scheme_changed_cb), NULL);

  gtk_source_buffer_set_highlight_syntax (GTK_SOURCE_BUFFER (buffer), TRUE);
  language = gtk_source_language_manager_get_language (LanguageManager, "scheme");
  gtk_source_buffer_set_language (GTK_SOURCE_BUFFER (buffer), language);
  gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER (buffer), TRUE);

  TextView = GTK_WIDGET (gtk_source_view_new_with_buffer (GTK_SOURCE_BUFFER (buffer)));

  g_object_set_data (G_OBJECT (TextView), "pfilename", filename);
  gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW (TextView), TRUE);
  gtk_source_view_set_auto_indent (GTK_SOURCE_VIEW (TextView), TRUE);
  gtk_source_view_set_indent_on_tab (GTK_SOURCE_VIEW (TextView), TRUE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (TextView), GTK_WRAP_CHAR);

  GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  GtkWidget *w = window;
  gtk_window_set_title (GTK_WINDOW (w), _("Denemo Scheme Script"));
  //gtk_window_set_resizable (GTK_WINDOW (w), TRUE);
  g_signal_connect (G_OBJECT (w), "delete-event", G_CALLBACK (hide_scheme /*gtk_widget_hide_on_delete */ ), w);
  GtkWidget *main_vbox = gtk_vbox_new (FALSE, 1);
  gtk_container_add (GTK_CONTAINER (w), main_vbox);

  GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
  w = gtk_button_new_with_label ("CLI: ");
  GtkWidget *button = w;
  //gtk_widget_set_can_default(w, TRUE);
  //GTK_WIDGET_SET_FLAGS(window, GTK_CAN_DEFAULT);
  //gtk_window_set_default (window, w);  
  gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, TRUE, 0);
  w = gtk_entry_new ();
  GtkWidget *entry = w;
  //gtk_entry_set_activates_default (w,TRUE);
  gtk_box_pack_start (GTK_BOX (hbox), w, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (main_vbox), hbox, FALSE, TRUE, 0);
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (executeCLI), entry);
  g_signal_connect (G_OBJECT (entry), "key-press-event", G_CALLBACK (keypress), NULL);
  w = gtk_button_new_with_label ("Execute Script");
  g_signal_connect (G_OBJECT (w), "clicked", G_CALLBACK (executeScript), NULL);
  gtk_box_pack_start (GTK_BOX (main_vbox), w, FALSE, FALSE, 0);


  menu = gtk_menu_new ();

  item = gtk_menu_item_new_with_label (_("New"));
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (clear_scheme_window), (gpointer) TextView);

  item = gtk_menu_item_new_with_label (_("Open"));
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (load_scheme_from_file), (gpointer) TextView);

  item = gtk_menu_item_new_with_label (_("Save"));
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (save_scheme_text), (gpointer) TextView);

  item = gtk_menu_item_new_with_label (_("Save as…"));
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (save_scheme_text_as), (gpointer) TextView);

  item = gtk_menu_item_new_with_label (_("Find"));
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (find_cb), (gpointer) TextView);
  item = gtk_menu_item_new_with_label (_("Replace"));
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (replace_cb), (gpointer) TextView);



  fileMenu = gtk_menu_item_new_with_label (_("File"));
  gtk_widget_show (fileMenu);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (fileMenu), menu);
  gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (buffer), FALSE);

  menuBar = gtk_menu_bar_new ();

  gtk_box_pack_start (GTK_BOX (main_vbox), menuBar, FALSE, FALSE, 0);
  gtk_widget_show (menuBar);
  gtk_menu_shell_append (GTK_MENU_SHELL (menuBar), fileMenu);

  GtkWidget *inner_hbox = gtk_hbox_new (FALSE, 1);
  gtk_box_pack_start (GTK_BOX (main_vbox), inner_hbox, FALSE, FALSE, 0);
  GtkWidget *wid = gtk_check_button_new ();
  gtk_activatable_set_related_action (GTK_ACTIVATABLE (wid), gtk_ui_manager_get_action (Denemo.ui_manager, "/MainMenu/HiddenMenu/RecordScript"));
  gtk_box_pack_start (GTK_BOX (inner_hbox), wid, FALSE, FALSE, 0);
  
  GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  gtk_box_pack_start (GTK_BOX (main_vbox), sw, TRUE, TRUE, 0);

  gtk_container_add (GTK_CONTAINER (sw), TextView);
  return TextView;
}


void
create_scheme_window (void)
{
  Denemo.script_view = create_editor_window ();
}



/* Buffer action callbacks ------------------------------------------------------------ */
#if  GTK_MAJOR_VERSION==3
static struct
{
  char *what;
  char *replacement;
  GtkTextSearchFlags flags;
} search_data =
{
NULL, NULL, GTK_TEXT_SEARCH_TEXT_ONLY};
#else
static struct
{
  char *what;
  char *replacement;
  GtkSourceSearchFlags flags;
} search_data =
{
NULL, NULL, GTK_SOURCE_SEARCH_CASE_INSENSITIVE};
#endif
#if GTK_MAJOR_VERSION==3
static gboolean
search_dialog (GtkWidget * widget, gboolean replace, char **what_p, char **replacement_p, GtkTextSearchFlags * flags_p)
#else
static gboolean
search_dialog (GtkWidget * widget, gboolean replace, char **what_p, char **replacement_p, GtkSourceSearchFlags * flags_p)
#endif
{
  GtkWidget *dialog;
  GtkEntry *entry1, *entry2;

  dialog = gtk_dialog_new_with_buttons (replace ? _("Replace") : _("Find"), GTK_WINDOW (gtk_widget_get_toplevel (widget)), GTK_DIALOG_MODAL, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);

  entry1 = g_object_new (GTK_TYPE_ENTRY, "visible", TRUE, "text", search_data.what ? search_data.what : "", "activates-default", TRUE, NULL);
  GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  GtkWidget *vbox = gtk_vbox_new (FALSE, 1);
  gtk_container_add (GTK_CONTAINER (content_area), vbox);
  gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (entry1), TRUE, TRUE, 0);
  entry2 = g_object_new (GTK_TYPE_ENTRY, "visible", replace, "text", search_data.replacement ? search_data.replacement : "", "activates-default", TRUE, NULL);
  gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (entry2), TRUE, TRUE, 0);

#if GTK_MAJOR_VERSION==2
  GtkToggleButton *case_sensitive;
  case_sensitive = g_object_new (GTK_TYPE_CHECK_BUTTON, "visible", TRUE, "label", "Case sensitive", "active", !(search_data.flags & GTK_SOURCE_SEARCH_CASE_INSENSITIVE), NULL);

  gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (case_sensitive), FALSE, FALSE, 0);
#endif
  gtk_widget_show_all (dialog);

  while (TRUE)
    {
      if (gtk_dialog_run (GTK_DIALOG (dialog)) != GTK_RESPONSE_OK)
        {
          gtk_widget_destroy (dialog);
          return FALSE;
        }

      if (*gtk_entry_get_text (entry1))
        break;
    }

  g_free (search_data.what);
  *what_p = search_data.what = g_strdup (gtk_entry_get_text (entry1));
  g_free (search_data.replacement);
  *replacement_p = search_data.replacement = g_strdup (gtk_entry_get_text (entry2));
#if GTK_MAJOR_VERSION==2
  *flags_p = search_data.flags = gtk_toggle_button_get_active (case_sensitive) ? 0 : GTK_SOURCE_SEARCH_CASE_INSENSITIVE;
#endif
  gtk_widget_destroy (dialog);
  return TRUE;
}

static void
do_search_replace (GtkTextView * view, gboolean replace)
{
  GtkTextBuffer *buffer = gtk_text_view_get_buffer (view);
  GtkTextIter iter;
  char *what, *replacement;
#if GTK_MAJOR_VERSION==3
  GtkTextSearchFlags flags;
#else
  GtkSourceSearchFlags flags;
#endif

  if (!search_dialog (GTK_WIDGET (view), replace, &what, &replacement, &flags))
    return;

  if (replace)
    {
      gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);

      while (TRUE)
        {
          GtkTextIter match_start, match_end;
#if GTK_MAJOR_VERSION==3
          if (!gtk_text_iter_forward_search (&iter, what, flags, &match_start, &match_end, NULL))
#else
          if (!gtk_source_iter_forward_search (&iter, what, flags, &match_start, &match_end, NULL))
#endif
            {
              break;
            }

          gtk_text_buffer_delete (buffer, &match_start, &match_end);
          gtk_text_buffer_insert (buffer, &match_start, replacement, -1);
          iter = match_start;
        }
    }
  else
    {
      GtkTextIter match_start, match_end;

      gtk_text_buffer_get_iter_at_mark (buffer, &iter, gtk_text_buffer_get_insert (buffer));
#if GTK_MAJOR_VERSION==3
      if (gtk_text_iter_forward_search (&iter, what, flags, &match_start, &match_end, NULL))
#else
      if (gtk_source_iter_forward_search (&iter, what, flags, &match_start, &match_end, NULL))
#endif
        {
          gtk_text_buffer_select_range (buffer, &match_start, &match_end);
        }
      else
        {
          GtkTextIter insert = iter;
          gtk_text_buffer_get_start_iter (buffer, &iter);
#if GTK_MAJOR_VERSION==3
          if (gtk_text_iter_forward_search (&iter, what, flags, &match_start, &match_end, &insert))
#else
          if (gtk_source_iter_forward_search (&iter, what, flags, &match_start, &match_end, &insert))
#endif
            gtk_text_buffer_select_range (buffer, &match_start, &match_end);
        }
    }
}

static void
find_cb (GtkAction * action, gpointer user_data)
{
  do_search_replace (user_data, FALSE);
}

static void
replace_cb (GtkAction * action, gpointer user_data)
{
  do_search_replace (user_data, TRUE);
}
