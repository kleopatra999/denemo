/* kbd-interface.c
 *  functions for implementing the customize keyboard dialog
 *
 *  for Denemo, thu GNU graphical music notation editor
 *  (c) 2000-2005 Matthew Hiller 
 */

#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "core/kbd-custom.h"
#include "core/prefops.h"
#include "ui/mousing.h"
#include "core/utils.h"


static gint
get_command_position(GtkTreeModel* model, gint command_idx){
  gboolean valid;
  command_row* row1 = NULL;
  command_row* row2 = NULL;
  gint i = 0;
  GtkTreeIter iter;
  
  valid = gtk_tree_model_get_iter_first (model, &iter);
  keymap_get_command_row(Denemo.map, &row2, command_idx);
  
  while (valid){
    gtk_tree_model_get (model, &iter, COL_ROW, &row1, -1);
    if(row1 && row1 == row2)
      return i;
    i++;
    valid = gtk_tree_model_iter_next (model, &iter);
  }
  return -1;
}

#if 0
static void
validate_keymap_name (GtkEntry * entry, GtkDialog * dialog)
{
  const gchar *name = gtk_entry_get_text (GTK_ENTRY (entry));
  gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT, strlen (name) > 0);
}
#endif

static gboolean
capture_add_binding (GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  GdkModifierType modifiers;
  guint command_idx;
  GtkTreeModel *model;
  GtkTreeSelection *selection;
  GtkTreeIter iter;
  GtkTreePath *path;
  gint *array;
  keyboard_dialog_data *cbdata = (keyboard_dialog_data *) user_data;
  //get the shortcut
  if (isModifier (event))
    return TRUE;
  dnm_clean_event (event);
  modifiers = dnm_sanitize_key_state (event);
  gchar *name = dnm_accelerator_name (event->keyval, event->state);
  if (cbdata->two_key == 1)
    {
      gint command_idx = lookup_command_for_keybinding_name (Denemo.map, name);
      if (command_idx == -1)
        {
          cbdata->first_keyval = event->keyval;
          cbdata->first_modifiers = modifiers;
          cbdata->two_key = 2;
          return TRUE;
        }
      else
        {
          cbdata->two_key = 0;
          gchar *msg = g_strdup_printf (_("The command %s has the shortcut: %s\nDelete it first or start again selecting an unused keypress."), lookup_name_from_idx (Denemo.map, command_idx), name);
          warningdialog (msg);
          g_free (msg);
          g_free (name);
          g_warning ("trying to set a two key starting with a single");
          return TRUE;
        }
    }
  //get the command_index
  selection = gtk_tree_view_get_selection (cbdata->command_view);
  gtk_tree_selection_get_selected (selection, &model, &iter);
  path = gtk_tree_model_get_path (model, &iter);
  array = gtk_tree_path_get_indices (path);
  command_idx = array[0]; //this fails in general, yielding the value in cbdata, instead ...
  gchar* cname = NULL;
  gtk_tree_model_get (model, &iter, COL_NAME, &cname, -1);
  command_idx = lookup_command_from_name (Denemo.map, cname);
  if(command_idx != cbdata->command_id)
    g_warning("correct command idx %d compare %d for action of name %s\n", command_idx, cbdata->command_id, cname);
  gtk_tree_path_free (path);


  if (cbdata->two_key == 0 && (GList *) g_hash_table_lookup (Denemo.map->continuations_table, name))
    {
      //g_warning("There is a two key binding starting with this");
      gchar *msg = g_strdup_printf (_("There is at least one two-key shortcut that starts with: %s\nFind them using the Find button\nDelete it/those first or start again selecting an unused keypress."), name);
      warningdialog (msg);
      g_free (msg);
      g_free (name);
      return TRUE;
    }
  g_free (name);
  //set the new binding
  if (cbdata->two_key == 2)
    add_twokeybinding_to_idx (Denemo.map, cbdata->first_keyval, cbdata->first_modifiers, event->keyval, modifiers, command_idx, POS_FIRST);
  else
    add_keybinding_to_idx (Denemo.map, event->keyval, modifiers, command_idx, POS_FIRST);

  command_row* row = NULL;
  keymap_get_command_row (Denemo.map, &row, command_idx);
  GtkListStore* bindings_model = GTK_LIST_STORE(gtk_tree_view_get_model(cbdata->binding_view));
  update_bindings_model(bindings_model, row->bindings);
  
  gtk_statusbar_pop (cbdata->statusbar, cbdata->context_id);
  g_signal_handler_disconnect (GTK_WIDGET (widget), cbdata->handler_key_press);
  g_signal_handler_disconnect (GTK_WIDGET (widget), cbdata->handler_focus_out);
  cbdata->two_key = 0;
  return TRUE;
}

static gboolean
capture_look_binding (GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  GdkModifierType modifiers;
  GtkTreeModel *model;
  GtkTreeSelection *selection;
  GtkTreeIter iter;
  GtkTreePath *path;
  gint command_idx;
  keyboard_dialog_data *cbdata = (keyboard_dialog_data *) user_data;
  gtk_statusbar_pop (cbdata->statusbar, cbdata->context_id);
  //get the shortcut
  if (isModifier (event))
    return TRUE;
  dnm_clean_event (event);
  modifiers = dnm_sanitize_key_state (event);

  //look for the keybinding
  command_idx = lookup_command_for_keybinding (Denemo.map, event->keyval, modifiers);
  //if the binding is associated to a command 
  if (command_idx == -1)
    {
      gchar *name = dnm_accelerator_name (event->keyval, event->state);
      GList *g = (GList *) g_hash_table_lookup (Denemo.map->continuations_table, name);

      GString *continuations = g_string_new ("");
      GString *final_list = g_string_new ("");
      for (; g; g = g->next)
        {
          g_string_append_printf (continuations, "%s%s%s", name, ",", (gchar *) g->data);
          command_idx = lookup_command_for_keybinding_name (Denemo.map, continuations->str);
          const gchar *this = lookup_name_from_idx (Denemo.map, command_idx);
          g_string_append_printf (final_list, "%s,%s=%s ", name, (gchar *) g->data, this);
          g_string_assign (continuations, "");
        }
      if (final_list->len)
        cbdata->twokeylist = final_list;
    }
  if (command_idx != -1)
  {
      model = gtk_tree_view_get_model (cbdata->command_view);
      gint pos = get_command_position(model, command_idx);
      selection = gtk_tree_view_get_selection (cbdata->command_view);
      gtk_tree_model_iter_nth_child (model, &iter, NULL, pos);
      gtk_tree_selection_select_iter (selection, &iter);
      path = gtk_tree_model_get_path (model, &iter);
      gtk_tree_view_scroll_to_cell (cbdata->command_view, path, NULL, FALSE, 0, 0);
      gtk_tree_path_free (path);
    }
  else
    gtk_statusbar_push (cbdata->statusbar, cbdata->context_id, _("No command has this keyboard shortcut"));
  if (cbdata->twokeylist)
    {
      gtk_statusbar_push (cbdata->statusbar, cbdata->context_id, cbdata->twokeylist->str);
      g_string_free (cbdata->twokeylist, TRUE);
      cbdata->twokeylist = NULL;
    }
  //clean the GUI
  g_signal_handler_disconnect (GTK_WIDGET (widget), cbdata->handler_key_press);
  g_signal_handler_disconnect (GTK_WIDGET (widget), cbdata->handler_focus_out);

  return TRUE;
}

static gboolean
stop_capture_binding (GtkWidget * widget, G_GNUC_UNUSED GdkEventFocus * event, gpointer user_data)
{
  keyboard_dialog_data *cbdata = (keyboard_dialog_data *) user_data;
  gtk_statusbar_pop (cbdata->statusbar, cbdata->context_id);
  g_signal_handler_disconnect (GTK_WIDGET (widget), cbdata->handler_key_press);
  g_signal_handler_disconnect (GTK_WIDGET (widget), cbdata->handler_focus_out);
  return FALSE;
}

static void
kbd_interface_add_binding (GtkButton * button, gpointer user_data)
{
  GtkTreeSelection *selection;
  keyboard_dialog_data *cbdata = (keyboard_dialog_data *) user_data;
  gtk_statusbar_pop (cbdata->statusbar, cbdata->context_id);
  // check a command is selected
  selection = gtk_tree_view_get_selection (cbdata->command_view);
  if (!gtk_tree_selection_get_selected (selection, NULL, NULL))
    return;
  gtk_statusbar_push (cbdata->statusbar, cbdata->context_id, _("Press a shortcut sequence for this command"));
  cbdata->handler_key_press = g_signal_connect (GTK_WIDGET (button), "key-press-event", G_CALLBACK (capture_add_binding), user_data);
  cbdata->handler_focus_out = g_signal_connect (GTK_WIDGET (button), "focus-out-event", G_CALLBACK (stop_capture_binding), user_data);
  Denemo.accelerator_status = TRUE;
}

static void
kbd_interface_add_2binding (GtkButton * button, gpointer user_data)
{
  keyboard_dialog_data *cbdata = (keyboard_dialog_data *) user_data;
  gtk_statusbar_pop (cbdata->statusbar, cbdata->context_id);
  cbdata->two_key = 1;
  kbd_interface_add_binding (button, user_data);
}

static void
kbd_interface_look_binding (GtkButton * button, gpointer user_data)
{
  keyboard_dialog_data *cbdata = (keyboard_dialog_data *) user_data;
  gtk_statusbar_pop (cbdata->statusbar, cbdata->context_id);
  gtk_statusbar_push (cbdata->statusbar, cbdata->context_id, _("Press a shortcut sequence whose command you seek"));
  cbdata->handler_key_press = g_signal_connect (GTK_WIDGET (button), "key-press-event", G_CALLBACK (capture_look_binding), user_data);
  cbdata->handler_focus_out = g_signal_connect (GTK_WIDGET (button), "focus-out-event", G_CALLBACK (stop_capture_binding), user_data);
}

static void
kbd_interface_del_binding (G_GNUC_UNUSED GtkButton * button, gpointer user_data)
{
  GtkTreeSelection *selection;
  gchar *binding;
  GtkTreeModel *model;
  GtkTreeIter iter;
  command_row* row = NULL;
  keyboard_dialog_data *cbdata = (keyboard_dialog_data *) user_data;
  gtk_statusbar_pop (cbdata->statusbar, cbdata->context_id);
  selection = gtk_tree_view_get_selection (cbdata->binding_view);
  //if no binding is selected, we do nothing
  if (!gtk_tree_selection_get_selected (selection, &model, &iter))
    return;
  //else get the binding and remove it
  gtk_tree_model_get (model, &iter, 0, &binding, -1);
  
  gint command_id_ptr = lookup_command_for_keybinding_name(Denemo.map, binding);
  remove_keybinding_from_name (Denemo.map, binding);

  if(command_id_ptr > -1){
    if(keymap_get_command_row (Denemo.map, &row, command_id_ptr))
      update_bindings_model(GTK_LIST_STORE(model), row->bindings);
  }
  else 
    g_debug("Cannot find command to delete.\n");
  g_free (binding);
  Denemo.accelerator_status = TRUE;
}

static void
execute_current (keyboard_dialog_data *data)
{
  GtkTreeModel *model;
  GtkTreeSelection *selection;
  GtkTreeIter iter;
  gchar* cname = NULL;
  selection = gtk_tree_view_get_selection (data->command_view);
  gtk_tree_selection_get_selected (selection, &model, &iter);
  gtk_tree_model_get (model, &iter, COL_NAME, &cname, -1);
  gint command_idx = lookup_command_from_name (Denemo.map, cname);
  if(command_idx != data->command_id)
    g_warning("correct command idx %d compare %d for action of name %s\n", command_idx, data->command_id, cname);
 execute_callback_from_idx (Denemo.map, command_idx);
}
static void
add_current_to_palette (keyboard_dialog_data *data)
{
 place_action_in_palette (data->command_id, NULL);
}




typedef struct ModifierPointerInfo
{
  guint button_mask;
  guint cursor_number;
} ModifierPointerInfo;

#define DENEMO_MODIFIER_MASK (255)
static void
keyboard_modifier_callback (GtkWidget * w, GdkEventButton * event, ModifierPointerInfo * info)
{
  gint mask = info->button_mask;
  gint state = (event->state & DENEMO_MODIFIER_MASK) | mask;
  gint cursor_number = info->cursor_number;
  GdkCursor *cursor = gdk_cursor_new (cursor_number);
  //g_hash_table_lookup(Denemo.map->cursors, &state);

  // show_type(w, "button mod callback: ");
  GString *str = g_string_new ("");
  g_string_append_printf (str, "Cursor Shape:- %d\n Mouse:- %s\nKeyboard:", cursor_number, mask ? (mask & GDK_BUTTON1_MASK ? "Left Button Drag" : "Right Button Drag") : "No Button Press");
  append_modifier_name (str, state);
#define POINTER_PROMPT  "To change the cursor shape for a mouse/keyboard state:\nSelect Cursor Shape number\nChoose mouse state and then click here\nwhile holding modifier key\nand/or engaging Caps/Num lock for the keyboard state. Finally save command set."
  gdk_window_set_cursor (gtk_widget_get_window (w), cursor);
//g_string_append(str, "\n");
//  g_string_append(str, POINTER_PROMPT);

  gtk_button_set_label ((GtkButton *) w, str->str);
  //gtk_widget_set_tooltip(w, POINTER_PROMPT);
  assign_cursor (state, cursor_number);
  g_string_free (str, TRUE);
}

/* returns a cursor number in *number from the passed spinner widget
 FIXME what is cursor doing? */
void
set_cursor_number (GtkSpinButton * widget, gint * number)
{
  *number = gtk_spin_button_get_value_as_int (widget);
  //static GdkCursor *cursor;
  //  if(cursor)
  //  g_object_unref(cursor);
  /*cursor = */gdk_cursor_new (*number);
  //gdk_window_set_cursor(widget->window, cursor);   
}

static void
button_choice_callback (GtkWidget * w, gint * mask)
{
  gint choice = gtk_toggle_button_get_active ((GtkToggleButton *) w);
  if (choice)
    *mask = (intptr_t) g_object_get_data ((GObject *) w, "mask");
  g_debug ("button choice %x\n", *mask);
}


static void
load_system_keymap_dialog_response (void)
{
  load_system_keymap_dialog ();
  
}

static void
load_keymap_dialog_response (void)
{
  load_keymap_dialog ();
  
}

static GtkWidget *SearchEntry = NULL;
static GtkWidget *SearchNext = NULL;

static  keyboard_dialog_data cbdata;

GtkWidget *get_command_view(void)
{
return  cbdata.command_view;
}

void
configure_keyboard_dialog_init_idx (GtkAction * action, gint command_idx)
{
  
  GtkWidget *frame;
  GtkWidget *vbox, *outer_hbox;
  GtkWidget *table;
  GtkWidget *label;
  GtkWidget *addbutton;
  GtkWidget *add2button;
  GtkWidget *delbutton;
  GtkWidget *lookbutton;
  GtkWidget *statusbar;
  GtkWidget *button_save;
  GtkWidget *button_save_as;
  GtkWidget *button_load;
  GtkWidget *button_load_from;

  GtkWidget *command_view;
  GtkWidget *binding_view;
  GtkWidget *command_tree_view;
  GtkWidget *binding_tree_view;
  GtkWidget *text_view;
  GtkWidget *scrolled_text_view;

  GtkTreeSelection *selection;
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreePath *path;
  guint context_id;
 
  if(Denemo.command_manager)   {
      model = gtk_tree_view_get_model (GTK_TREE_VIEW (cbdata.command_view));
      if (command_idx == -1)
        {
        //selecting the first command
        gtk_tree_model_get_iter_first (model, &iter);
        } 
      else
        {
      gint pos = get_command_position(model, command_idx);
      gtk_tree_model_iter_nth_child (model, &iter, NULL, pos);
        }
      gtk_widget_grab_focus (SearchEntry);
      selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (cbdata.command_view));
      gtk_tree_selection_select_iter (selection, &iter);
      path = gtk_tree_model_get_path (model, &iter);
      gtk_tree_view_scroll_to_cell ((GtkTreeView *) cbdata.command_view, path, NULL, FALSE, 0, 0);
      gtk_tree_path_free (path);
      if(!gtk_widget_get_visible(Denemo.command_manager))
        activate_action ("/MainMenu/ViewMenu/" "ToggleCommandManager");
      return;
    }
   if(SearchEntry==NULL) {
    SearchEntry = gtk_entry_new ();
    SearchNext = gtk_button_new_with_label ("→");
    gtk_widget_set_tooltip_text (SearchEntry, _("Type search text here. Either the start of a command label,\nor part of the text of a tooltip (if the tooltips flag is set).\nThe search is case insensitive."));
    gtk_widget_set_tooltip_text (SearchNext, _("Search for the next matching command"));
}   
  //getting a binding view and a command view and connecting the change of
  //command selection the the change of the model displayed by the binding view
  binding_view = keymap_get_binding_view ();
  binding_tree_view = gtk_bin_get_child (GTK_BIN (binding_view));
  command_view = GTK_WIDGET (keymap_get_command_view (Denemo.map, SearchEntry, SearchNext));
  command_tree_view = gtk_bin_get_child (GTK_BIN (command_view));

  Denemo.command_manager = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(Denemo.command_manager), (_("Command Center")));
  if (Denemo.prefs.newbie)
    gtk_widget_set_tooltip_text (Denemo.command_manager,
                                 _
                                 ("This window allows you find and execute commands.\nOnce you have selected a command the box at the top gives information about that command,"
                                 "\nwhile the panel to the side gives any shortcuts for the command"
                                 "\nyou can add a selected command to a palette, or set single-key or two-key shortcuts."
                                 "\nYou can also hide commands, so they don't appear in the menus.\n"
                                 "You can save the shortcuts as your default command set, or as a command set which you may wish to load in the future.\n"
                                 "This window is also where you can load such a stored command set."));

  outer_hbox = gtk_hbox_new (FALSE, 8);
  gtk_container_add (GTK_CONTAINER (Denemo.command_manager), outer_hbox);
  vbox = gtk_vbox_new (FALSE, 8);
   
  

  gtk_container_set_border_width (GTK_CONTAINER (vbox), 12);
  gtk_box_pack_start (GTK_BOX (outer_hbox), vbox, TRUE, TRUE, 0);

    
  frame = gtk_frame_new (_("Help for Selected Command"));
  gtk_frame_set_shadow_type ((GtkFrame *) frame, GTK_SHADOW_IN);
  gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);
  text_view = gtk_text_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (text_view), FALSE);
  scrolled_text_view = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (scrolled_text_view), text_view);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_text_view), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (frame), scrolled_text_view);


  {
  GtkWidget *inner_hbox = gtk_hbox_new (FALSE, 8);
  gtk_box_pack_start (GTK_BOX (vbox), inner_hbox, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (inner_hbox), gtk_widget_get_parent(command_view), TRUE, TRUE, 0);
  {
    GtkWidget *inner_vbox = gtk_vbox_new (FALSE, 8);
  gtk_box_pack_start (GTK_BOX (inner_hbox), inner_vbox,  TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (inner_vbox), binding_view,  TRUE, TRUE, 0);
  delbutton = gtk_button_new_from_stock (GTK_STOCK_REMOVE);
    gtk_box_pack_start (GTK_BOX (inner_vbox), delbutton, FALSE, TRUE, 0);
}
  }
  //lookbutton = gtk_button_new_from_stock (GTK_STOCK_FIND);
  // gtk_table_attach (GTK_TABLE (table), lookbutton, 5, 6, 5, 6,
//                  (GtkAttachOptions) (GTK_FILL),
//                  (GtkAttachOptions) (0), 0, 0);

  vbox = gtk_vbox_new (FALSE, 8);
  gtk_box_pack_end (GTK_BOX (outer_hbox), vbox, FALSE, TRUE, 0);
  
   {
  GtkWidget *inner_hbox = gtk_hbox_new (FALSE, 8);
  gtk_box_pack_start (GTK_BOX (vbox), inner_hbox, FALSE, TRUE, 0);
   button_save = gtk_button_new_with_label (_("Save as Default Command Set"));
    gtk_box_pack_start (GTK_BOX (inner_hbox), button_save, FALSE, TRUE, 0);
  gtk_widget_set_tooltip_text (button_save, _("Use this to save the changes you have made so that they are used every time you start Denemo. The changes are stored under a directory (folder) called .denemo-* in your home directory. Look in subdirectory actions for Default.commands"));
  button_save_as = gtk_button_new_with_label (_("Save as a Custom Command Set"));
    gtk_box_pack_start (GTK_BOX (inner_hbox), button_save_as, FALSE, TRUE, 0);

  button_load = gtk_button_new_with_label (_("Load a Standard Command Set"));
    inner_hbox = gtk_hbox_new (FALSE, 8);
  gtk_box_pack_start (GTK_BOX (vbox), inner_hbox, FALSE, TRUE, 0);
    button_load = gtk_button_new_with_label (_("Load a Standard Command Set"));
    gtk_box_pack_start (GTK_BOX (inner_hbox), button_load, FALSE, TRUE, 0);

  button_load_from = gtk_button_new_with_label (_("Load a Custom Command Set"));
    gtk_box_pack_start (GTK_BOX (inner_hbox), button_load_from, FALSE, TRUE, 0);

   }
  
  GtkWidget *inner_hbox = gtk_hbox_new (FALSE, 1);
  gtk_box_pack_end (GTK_BOX (vbox), inner_hbox, FALSE, TRUE, 0);
    GtkWidget *inner_vbox = gtk_vbox_new (FALSE, 1);
  gtk_box_pack_end (GTK_BOX (inner_hbox), inner_vbox, FALSE, TRUE, 0);
  addbutton = gtk_button_new_from_stock (GTK_STOCK_ADD);
  gtk_button_set_label (GTK_BUTTON (addbutton), _("Add 1-Key Shortcut"));
  gtk_widget_set_tooltip_text (addbutton, _("Create a single keypress (with modifier keys - Control, Shift ... - if needed) as a keyboard shortcut for the currently selected command."));
  gtk_box_pack_end (GTK_BOX (inner_vbox), addbutton, FALSE, TRUE, 0);

  add2button = gtk_button_new_from_stock (GTK_STOCK_ADD);
  gtk_button_set_label (GTK_BUTTON (add2button), _("Add 2-Key Shortcut"));
  gtk_widget_set_tooltip_text (add2button, _("Create a two keypress sequence as a keyboard shortcut for the currently selected command."));
  gtk_box_pack_end (GTK_BOX (inner_vbox), add2button, FALSE, TRUE, 0);

  lookbutton = gtk_button_new_from_stock (GTK_STOCK_FIND);
  gtk_button_set_label (GTK_BUTTON (lookbutton), _("Find Command for Shortcut"));
  gtk_widget_set_tooltip_text (lookbutton, _("Finds the command (if any) for a (one key) Keyboard Shortcut\n"
                                              "Click button then press the key shortcut you are looking for."));
  gtk_box_pack_end (GTK_BOX (inner_vbox), lookbutton, FALSE, TRUE, 0);


  inner_vbox = gtk_vbox_new (FALSE, 1);
  gtk_box_pack_start (GTK_BOX (inner_hbox), inner_vbox, FALSE, TRUE, 0);
  GtkWidget *execute_button = gtk_button_new_with_label (_("Execute Selected Command"));
  gtk_widget_set_tooltip_text (execute_button, _("Executes the currently selected command in the list of commands\nEnsure the cursor is in the movement and at the position if needed for the command."));
  gtk_box_pack_end (GTK_BOX (inner_vbox), execute_button, FALSE, TRUE, 0);
  GtkWidget *palette_button = gtk_button_new_with_label (_("Add to Palette"));
  gtk_widget_set_tooltip_text (palette_button, _("Adds the currently selected command in the list of commands to a palette\nYou can create a new, custom palette, and you can change the label of the button you create by right-clicking on it."));
  gtk_box_pack_end (GTK_BOX (inner_vbox), palette_button, FALSE, TRUE, 0);
  g_signal_connect_swapped (G_OBJECT (execute_button), "clicked", G_CALLBACK (execute_current), &cbdata);
  g_signal_connect_swapped (G_OBJECT (palette_button), "clicked", G_CALLBACK (add_current_to_palette), &cbdata);

  
  
  
  
  statusbar = gtk_statusbar_new ();
  context_id = gtk_statusbar_get_context_id (GTK_STATUSBAR (statusbar), "");
  //FIXME gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(statusbar), FALSE);
  gtk_box_pack_end (GTK_BOX (vbox), statusbar, FALSE, TRUE, 0);

  cbdata.addbutton = GTK_BUTTON (addbutton);
  cbdata.add2button = GTK_BUTTON (add2button);
  cbdata.delbutton = GTK_BUTTON (delbutton);
  cbdata.lookbutton = GTK_BUTTON (lookbutton);
  cbdata.statusbar = GTK_STATUSBAR (statusbar);
  cbdata.context_id = context_id;
  cbdata.command_view = GTK_TREE_VIEW (command_tree_view);
  cbdata.binding_view = GTK_TREE_VIEW (binding_tree_view);
  cbdata.text_view = GTK_TEXT_VIEW (text_view);
  cbdata.command_id = -1;
  cbdata.two_key = 0;
  cbdata.twokeylist = NULL;

  //setup the link between command_view and binding_view
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (command_tree_view));
  gtk_tree_selection_set_select_function (selection, keymap_change_binding_view_on_command_selection, &cbdata, NULL);

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (command_tree_view));
  if (command_idx == -1)
    {
      //selecting the first command
      gtk_tree_model_get_iter_first (model, &iter);
    }
  else
    {
      gint pos = get_command_position(model, command_idx);
      gtk_tree_model_iter_nth_child (model, &iter, NULL, pos);
      path = gtk_tree_model_get_path (model, &iter);
      gtk_tree_view_scroll_to_cell ((GtkTreeView *) command_tree_view, path, NULL, FALSE, 0, 0);
      gtk_tree_path_free (path);
    }
  gtk_widget_grab_focus (SearchEntry);
  gtk_tree_selection_select_iter (selection, &iter);
  
  
  frame = gtk_frame_new (_("Setting the Cursor Shape for Mouse Ops"));
  gtk_frame_set_shadow_type ((GtkFrame *) frame, GTK_SHADOW_IN);
  //gtk_container_add (GTK_CONTAINER (vbox), frame);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, TRUE, 0);
  GtkWidget *hbox = gtk_hbox_new (FALSE, 8);
  gtk_container_add (GTK_CONTAINER (frame), hbox);

  vbox = gtk_vbox_new (FALSE, 8);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, FALSE, TRUE, 0);

  GtkWidget *cursor_button = gtk_button_new_with_label (_("Cursor Shape 0.\nMouse Operation Right Drag.\nKeyboard: None."));
  gtk_widget_set_tooltip_text (cursor_button, POINTER_PROMPT);
  static ModifierPointerInfo info;
  info.button_mask = GDK_BUTTON3_MASK;  //radio button for left, right none
  g_signal_connect (G_OBJECT (cursor_button), "button-release-event", G_CALLBACK (keyboard_modifier_callback), &info);
  gtk_box_pack_end (GTK_BOX (hbox), cursor_button, FALSE, TRUE, 0);

  GtkWidget *mouse_state = gtk_radio_button_new_with_label (NULL, _("Right Drag"));
  gtk_box_pack_start (GTK_BOX (vbox), mouse_state, FALSE, TRUE, 0);
  g_object_set_data ((GObject *) mouse_state, "mask", (gpointer) GDK_BUTTON3_MASK);
  g_signal_connect (mouse_state, "toggled", (GCallback) button_choice_callback, &info.button_mask);
  GtkWidget *mouse_state2 = gtk_radio_button_new_with_label_from_widget ((GtkRadioButton *) mouse_state, _("Mouse Move"));
  g_signal_connect (mouse_state2, "toggled", (GCallback) button_choice_callback, &info.button_mask);
  g_object_set_data ((GObject *) mouse_state2, "mask", 0);
  gtk_box_pack_start (GTK_BOX (vbox), mouse_state2, FALSE, TRUE, 0);
  mouse_state2 = gtk_radio_button_new_with_label_from_widget ((GtkRadioButton *) mouse_state, _("Left Drag"));
  g_signal_connect (mouse_state2, "toggled", (GCallback) button_choice_callback, &info.button_mask);
  g_object_set_data ((GObject *) mouse_state2, "mask", (gpointer) GDK_BUTTON1_MASK);
  gtk_box_pack_start (GTK_BOX (vbox), mouse_state2, FALSE, TRUE, 0);

  {
  GtkWidget *inner_hbox = gtk_hbox_new (FALSE, 8);
  gtk_box_pack_start (GTK_BOX (vbox), inner_hbox, FALSE, TRUE, 0);
  label = gtk_label_new (_("Cursor Shape #"));
  gtk_box_pack_start (GTK_BOX (inner_hbox), label, FALSE, TRUE, 0);
  GtkWidget *spinner_adj = (GtkWidget *) gtk_adjustment_new (info.cursor_number, 0.0, (gdouble) GDK_LAST_CURSOR - 1,
                                                             1.0, 1.0, 1.0);
  GtkWidget *spinner = gtk_spin_button_new ((GtkAdjustment *) spinner_adj, 1.0, 0);
  gtk_box_pack_start (GTK_BOX (inner_hbox), spinner, TRUE, TRUE, 0);
  g_signal_connect (G_OBJECT (spinner), "value-changed", G_CALLBACK (set_cursor_number), &info.cursor_number);
  }

  //FIXME here use gdk_cursor_get_image() to show the cursor selected.


  //Connecting signals
  g_signal_connect (addbutton, "clicked", G_CALLBACK (kbd_interface_add_binding), &cbdata);
  g_signal_connect (add2button, "clicked", G_CALLBACK (kbd_interface_add_2binding), &cbdata);

  g_signal_connect (lookbutton, "clicked", G_CALLBACK (kbd_interface_look_binding), &cbdata);
  g_signal_connect (delbutton, "clicked", G_CALLBACK (kbd_interface_del_binding), &cbdata);

  g_signal_connect (G_OBJECT (button_save), "clicked", G_CALLBACK (save_default_keymap_file), NULL);
  g_signal_connect (G_OBJECT (button_save_as), "clicked", G_CALLBACK (save_keymap_dialog), NULL);
  g_signal_connect (G_OBJECT (button_load), "clicked", G_CALLBACK (load_system_keymap_dialog_response), NULL);
  g_signal_connect (G_OBJECT (button_load_from), "clicked", G_CALLBACK (load_keymap_dialog_response), NULL);
 
  gtk_widget_show_all (Denemo.command_manager);
  g_signal_connect (Denemo.command_manager, "delete-event", G_CALLBACK (keymap_cleanup_command_view), &cbdata);
//!!!!  gtk_main(); not needed now??? but Denemo.command_manager needs setting to NULL if we actually allow destroy.

  //When closing the dialog remove the signals that were associated to the
  //dialog
 //FIXME is this needed??? keymap_cleanup_command_view (&cbdata);
 
}

void
configure_keyboard_dialog (GtkAction * action, DenemoScriptParam * param)
{
  configure_keyboard_dialog_init_idx (action, -1);
}
