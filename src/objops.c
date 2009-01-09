/* objops.cpp
 * functions that do operations to mudela objects
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 1999-2005 Matthew Hiller
 */

#include <denemo/denemo.h>
#include "chordops.h"
#include "utils.h"
#include "commandfuncs.h"
#include "objops.h"
#include "staffops.h"
#include <string.h>

void
initkeyaccs (gint * accs, gint number);
/**
 * Free the given object
 * @param mudobj the DenemoObject to free
 */
void
freeobject (DenemoObject * mudobj)
{
  if (mudobj)
    switch (mudobj->type)
      {
      case CHORD:
	freechord (mudobj);	/* Which also frees mudobj itself */
	break;
      default:
	g_free (mudobj);
	break;
      }
}

/**
 * Create a new timesignature object
 * @param time1 nominator of time signature
 * @param time2 denominator of the time signature
 * @return the timesignature
 */
DenemoObject *
dnm_newtimesigobj (gint time1, gint time2)
{
  DenemoObject *ret;
  timesig *newtimesig = (timesig *) g_malloc (sizeof (timesig));
  ret = (DenemoObject *) g_malloc (sizeof (DenemoObject));
  ret->type = TIMESIG;
  newtimesig->time1 = time1;
  newtimesig->time2 = time2;
  ret->object = newtimesig;
  set_basic_numticks (ret);
  setpixelmin (ret);
  return ret;
}



/**
 * Create a new clef object
 * @param type clef type to create 
 *
 * @return the clef 
 */
DenemoObject *
dnm_newclefobj (enum clefs type)
{
  DenemoObject *ret;
  clef *newclef = (clef *) g_malloc (sizeof (clef));
  ret = (DenemoObject *) g_malloc (sizeof (DenemoObject));
  ret->type = CLEF;
  ret->isinvisible = FALSE;
  newclef->type = type;
  ret->object = newclef;
  set_basic_numticks (ret);
  setpixelmin (ret);
  return ret;
}


void
set_modeaccs (gint * accs, gint number, gint mode)
{

  g_print ("Mode %d : %d \n", number, mode);
  if (mode == 0)
    {
      switch (number)
	{
	case 11:
	  number -= 7;
	  break;
	}
      initkeyaccs (accs, number);

    }
  else if (mode == 2)
    initkeyaccs (accs, 0);
  else if (number == 1 && mode == 1)
    initkeyaccs (accs, 0);
  else if (number == 1 && mode == 1)
    initkeyaccs (accs, 0);
  else if (number == 1 && mode == 1)
    initkeyaccs (accs, 0);
  else if (number == 1 && mode == 1)
    initkeyaccs (accs, 0);
  else if (number == 1 && mode == 1)
    initkeyaccs (accs, 0);
  else if (number == 1 && mode == 1)
    initkeyaccs (accs, 0);
}

/** 
 * This function initializes the accidental-context array associated with
 * a key signature or a staff to that appropriate for _number_ 
 */
void
initkeyaccs (gint * accs, gint number)
{
  int index;
  guint i;
  memset (accs, 0, SEVENGINTS);
  if (number > 0)
    for (index = 3; number; number--, index = (index + 4) % 7)
      accs[index] = 1;
  else if (number < 0)
    for (index = 6; number; number++, index = (index + 3) % 7)
      accs[index] = -1;

}

/**
 * Create a new keysignature object
 * 
 * @param number number of keysignature
 * @param isminor signifies if the key sig should be minor
 * @param mode    description of the keys mode
 * @return the key signature
 */
DenemoObject *
dnm_newkeyobj (gint number, gint isminor, gint mode)
{
  DenemoObject *ret;
  keysig *newkeysig = (keysig *) g_malloc (sizeof (keysig));
  ret = (DenemoObject *) g_malloc (sizeof (DenemoObject));
  ret->type = KEYSIG;
  ret->isinvisible = FALSE;
#ifdef DEBUG
  g_print ("Number %d \t IsMinor %d \t Mode %d\n", number, isminor, mode);
#endif

  newkeysig->mode = mode;
  newkeysig->number = number;
  newkeysig->isminor = isminor;



  if (isminor == 2)
    set_modeaccs (newkeysig->accs, number, mode);
  else
    initkeyaccs (newkeysig->accs, number);

  ret->object = newkeysig;
  set_basic_numticks (ret);
  setpixelmin (ret);
  return ret;
}

/**
 * Create a new measure break object
 * @return the measurebreak
 */
DenemoObject *
newmeasurebreakobject ()
{
  DenemoObject *ret;

  ret = (DenemoObject *) g_malloc (sizeof (DenemoObject));
  ret->type = MEASUREBREAK;
  return ret;
}

/**
 * Create a new staff break object
 * @return the staffbreak
 */
DenemoObject *
newstaffbreakobject ()
{
  DenemoObject *ret;
  ret = (DenemoObject *) g_malloc (sizeof (DenemoObject));
  ret->type = STAFFBREAK;
  return ret;
}


/**
 * Create a clone of the given object
 * @param orig the object to clone
 * @return the cloned object
 */
DenemoObject *
dnm_clone_object (DenemoObject * orig)
{
  DenemoObject *ret;
  if (orig != NULL) {
	  switch (orig->type)
	    {
	    case CHORD:
	      ret = clone_chord (orig);
	      break;
	    case LILYDIRECTIVE: {
	      lilydirective *curlily = (lilydirective*)orig->object;
	      ret = lily_directive_new(curlily->directive->str);
	      lilydirective *newlily = (lilydirective *)ret->object;
	      if(curlily->display && curlily->display->len)
		newlily->display = g_string_new(curlily->display->str);
	      ret->minpixelsalloted = orig->minpixelsalloted;
	      newlily->locked = curlily->locked;
		 }
	      break;
	    default:
	      ret = (DenemoObject *) g_malloc (sizeof (DenemoObject));
	      memcpy (ret, orig, sizeof (DenemoObject));//dangerous ret->object is duplicated pointer
	      break;
	    }
  }
  return ret;
}

/**
 *  Create a new stem directive 
 *  @param type the stem directive type
 *  @return the stem directive
 */
DenemoObject *
dnm_stem_directive_new (enum stemdirections type)
{
  DenemoObject *ret;
  stemdirective *newstemdir =
    (stemdirective *) g_malloc (sizeof (stemdirective));
  ret = (DenemoObject *) g_malloc (sizeof (DenemoObject));
  ret->type = STEMDIRECTIVE;
  ret->isinvisible = FALSE;
  newstemdir->type = type;
  ret->object = newstemdir;
  set_basic_numticks (ret);
  setpixelmin (ret);
  return ret;
}

/**
 * Create a new lilypond directive 
 *
 * @param type the lilypond directive body
 * @return the lilypond directive
 *
*/
DenemoObject *
lily_directive_new (gchar * type)
{
  DenemoObject *ret;
  lilydirective *newlily =
    (lilydirective *) g_malloc (sizeof (lilydirective));
  ret = (DenemoObject *) g_malloc (sizeof (DenemoObject));
  ret->type = LILYDIRECTIVE;
  newlily->directive = g_string_new (type);
  ret->object = newlily;
  set_basic_numticks (ret);
  setpixelmin (ret);
  return ret;
}


/**
 * Create a new dynamic object
 * @param type the dynamic to create
 * @return the dynamic
  */
DenemoObject *
dynamic_new (gchar * type)
{
  DenemoObject *ret;
  dynamic *newdyn = (dynamic *) g_malloc (sizeof (dynamic));
  ret = (DenemoObject *) g_malloc (sizeof (DenemoObject));
  ret->type = DYNAMIC;
  ret->isinvisible = FALSE;
  newdyn->type = g_string_new (type);
  ret->object = newdyn;
  set_basic_numticks (ret);
  setpixelmin (ret);
  return ret;
}

/**
 * Create a new lyric object
 * @param type the lyric to create
 * @param position whether it shoul be centered or not
 * @param syllable whether it is a syllable
 * @return the dynamic
  */
DenemoObject *
dnm_lyric_new (gchar * type, gint position, gboolean syllable)
{
  DenemoObject *ret;
  lyric *newlyric = (lyric *) g_malloc (sizeof (lyric));
  ret = (DenemoObject *) g_malloc (sizeof (DenemoObject));
  ret->type = LYRIC;
  ret->isinvisible = FALSE;


  newlyric->lyrics = g_string_new (type);
  newlyric->position = position;
  newlyric->is_syllable = syllable;
  ret->object = newlyric;
  set_basic_numticks (ret);
  setpixelmin (ret);
  return ret;
}

/**
 *  Create a DenemoObject
 * @param type DenemoObject type 
 * @return the DenemoObject
 */

DenemoObject *
dnm_newobj (DenemoObjType type)
{
  DenemoObject *ret = (DenemoObject *) g_malloc0 (sizeof (DenemoObject));;
  ret->type = type;
  set_basic_numticks (ret);
  setpixelmin (ret);  /* these do nothing at present - but if we introduce
			 a show markers option then we will want to allot 
			 some space
		      */
  return ret;
}

/**
 * Set the key signature into the score
 * @param curstaff the current staff
 * @param tokey the key sig to insert
 * @param type  major/minor/mode
 */
void
dnm_setinitialkeysig (DenemoStaff * curstaff, gint tokey, gint type)
{

  curstaff->skey = tokey;
  curstaff->skey_isminor = type;

  initkeyaccs (curstaff->leftmost_keyaccs, tokey);
  memcpy (curstaff->skeyaccs, curstaff->leftmost_keyaccs, SEVENGINTS);

  showwhichaccidentalswholestaff (curstaff);

}
