#include <glib.h>
#include <unistd.h>

/* Integration tests are those which launch the program in different 
 * environments, and with different parameters.
 */

#define DENEMO "../src/denemo"
#define DATA_DIR "integration-data"
#define TEMP_DIR "integration-tmp"

/*******************************************************************************
 * SETUP AND TEARDOWN
 ******************************************************************************/

static void
setup(gpointer fixture, gconstpointer data)
{
  if(!g_file_test(TEMP_DIR, G_FILE_TEST_EXISTS)){
    if(g_mkdir(TEMP_DIR, 0777) < 0)
      g_warning("Could not create " TEMP_DIR);
  }
}

static void
teardown(gpointer fixture, gconstpointer data)
{  
  if(g_file_test(TEMP_DIR, G_FILE_TEST_EXISTS)){
    if(g_remove(TEMP_DIR) < 0)
      g_warning("Could not remove " TEMP_DIR);
  }
}

/*******************************************************************************
 * TEST FUNCTIONS
 ******************************************************************************/

/** test_run_and_quit:
 * This is the simpliest test. It just launches denemo and quit.
 */
static void
test_run_and_quit (gpointer fixture, gconstpointer data)
{
  if (g_test_trap_fork (0, 0))
    {
      execl(DENEMO, DENEMO, "-n", "-a", "(d-Quit)", NULL);
      g_warn_if_reached ();
    }
  g_test_trap_assert_passed ();
}

/** test_open_blank_file
 * Opens a blank file and quits.
 */
static void
test_open_blank_file(gpointer fixture, gconstpointer data)
{
  if (g_test_trap_fork (0, 0))
    {
      execl(DENEMO, DENEMO, "-n", DATA_DIR "/blank.denemo", NULL);
      g_warn_if_reached ();
    }
  g_test_trap_assert_passed ();
}

/** test_open_save_blank_file
 * Opens a blank file, saves it and quit. The output file should be the same as
 * the input one.
 */
static void
test_open_save_blank_file(gpointer fixture, gconstpointer data)
{
  const gchar* output = TEMP_DIR "/blank.denemo";
  const gchar* input  = DATA_DIR "/blank.denemo";
  gchar* input_contents = NULL;
  gchar* output_contents = NULL;

  if (g_test_trap_fork (0, 0))
    {
      gchar* scheme = g_strdup_printf("(d-SaveAs \"%s\")(d-Quit)", output);
      execl(DENEMO, DENEMO, "-n", "-a", scheme, input, NULL);
      g_warn_if_reached ();
    }
  g_test_trap_assert_passed ();

  g_assert(g_file_test(output, G_FILE_TEST_EXISTS));
  g_remove(output);
  /* TODO:
  g_file_get_contents(input, &input_contents, NULL, NULL);
  g_file_get_contents(output, &output_contents, NULL, NULL);
  g_assert_cmpstr(input_contents, ==, output_contents);
  */
}

/*******************************************************************************
 * MAIN
 ******************************************************************************/

int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  if(!g_file_test(DENEMO, G_FILE_TEST_EXISTS))
    g_error("Denemo has not been compiled successfully");

  g_test_add ("/integration/run-and-quit", void, NULL, setup, test_run_and_quit, teardown);
  g_test_add ("/integration/open-blank-file", void, NULL, setup, test_open_blank_file, teardown);
  g_test_add ("/integration/open-and-save-blank-file", void, NULL, setup, test_open_save_blank_file, teardown);

  return g_test_run ();
}