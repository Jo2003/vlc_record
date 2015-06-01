#ifndef MACNOSLEEP_H
   #define MACNOSLEEP_H

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
//
//! \brief   put Mac to no sleep mode
//
//! \author  Jo2003
//! \date    01.06.2015
//
//! \return  0 -> ok, -1 -> error
//---------------------------------------------------------------------------
int macNoSleep();

//---------------------------------------------------------------------------
//
//! \brief   stop no sleep mode
//
//! \author  Jo2003
//! \date    01.06.2015
//
//! \return  0 -> ok, -1 -> error
//---------------------------------------------------------------------------
int macSleep();

//---------------------------------------------------------------------------
//
//! \brief   check if no sleep mode is active
//
//! \author  Jo2003
//! \date    01.06.2015
//
//! \return  1 -> active; 0 -> not active
//---------------------------------------------------------------------------
int macNoSleepActive();

#ifdef __cplusplus
}
#endif

#endif // MACNOSLEEP_H
