/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 24.02.2010 / 10:41:34
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __030410__PLAYSTATES_H
   #define __030410__PLAYSTATES_H

namespace IncPlay
{
   enum ePlayStates
   {
      PS_READY,
      PS_OPEN,
      PS_BUFFER,
      PS_PLAY,
      PS_PAUSE,
      PS_STOP,
      PS_END,
      PS_ERROR,
      PS_RECORD,
      PS_TIMER_RECORD,
      PS_TIMER_STBY,
      PS_WTF    = 255 // ;-)
   };
}

//===================================================================
/// \brief Namespace for buttons / button functions
//===================================================================
namespace Button
{
   enum eBtnRole
   {
      Play,            ///< play role
      Stop,            ///< stop role
      Pause,           ///< pause role
      Stop_and_Save,   ///< stop and save position
      Unknown          ///< unknown role
   };
}

#endif // __030410__PLAYSTATES_H
/************************* History ***************************\
| $Log$
\*************************************************************/

