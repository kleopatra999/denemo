/* denemo.h
 *
 * the important data structures and defines for denemo, a gtk+ frontend to
 * Lilypond, the GNU music typesetter
 *
 * (c) 1999-2005 Matthew Hiller, Adam Tee
 *
 */

#ifndef DENEMO_DATASTRUCTURES
#define DENEMO_DATASTRUCTURES
#ifdef __cplusplus
extern "C" {
#endif

/* Include the internationalization stuff.  */
#include <libintl.h>
#include <locale.h>

#define _(String) gettext (String)
#ifndef gettext_noop
# define gettext_noop(String) String
#endif
#define N_(String) gettext_noop (String)


#include <glib.h>
#include <gmodule.h>

#include "denemo_api.h"

#define LILYPOND_VERSION "2.8.7" /* version of lilypond that we output */

/*Set maximum number of undos*/
#define MAX_UNDOS 50

/* This unit of memory size is used all over the place.  */
#define SEVENGINTS (sizeof (gint) * 7)



/*Forward Declaration*/
typedef struct _PluginData PluginData;
/**
 * Structure borrowed from Gnumeric which 
 * allows us to unload a plugin.
 * This should render the denemo_plugin struct redundant.
 */
struct _PluginData {
  GModule *handle;
  void (*initialise) (DenemoGUI *si, PluginData *pd);
  void (*clean_up)   (DenemoGUI *si, PluginData *pd);
  gchar *title;
};


#define g_malloc(a) g_malloc0(a) /* for extensible debuggable code */
#if 0
  //use this to detect bad access to G_OBJECT
#define g_object_get_data(a,b) (G_IS_OBJECT(a)? g_object_get_data(a,b):((gpointer)fprintf(stderr,"Bad G_OBJECT at %s line %d\n",__FILE__, __LINE__), NULL))
#define gtk_action_get_name(a) (GTK_IS_ACTION(a)? gtk_action_get_name(a):((gpointer)fprintf(stderr,"Bad GTK_ACTION at %s line %d\n",__FILE__, __LINE__), NULL))


#endif
#if 0
  // use these to test for bad frees.
#define g_free
#define g_list_free
#define g_error_free
#define gtk_tree_path_free
#define g_string_free
#endif

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef DENEMO_DATASTRUCTURES  */
