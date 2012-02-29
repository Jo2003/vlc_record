/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     small_helpers.h
 *
 *  @author   Jo2003
 *
 *  @date     29.02.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20120229_SMALL_HELPERS_H
   #define __20120229_SMALL_HELPERS_H

//---------------------------------------------------------------------------
/// a place for small helpers ...
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//! \brief   check if value is in between a min and max range
//
//! \author  Jo2003
//! \date    29.02.2012
//
//! \param   min minimal value
//! \param   max maximum value
//! \param   val value to check
//
//! \return  true --> in range
//! \return  false --> out of range
//---------------------------------------------------------------------------
template <typename T>
bool inBetween(const T &min, const T &max, const T &val)
{
   return ((val >= min) && (val <= max)) ? true : false;
}

//---------------------------------------------------------------------------
//
//! \brief   get larger value back
//
//! \author  Jo2003
//! \date    29.02.2012
//
//! \param   a first value
//! \param   b second value
//
//! \return  larger value
//---------------------------------------------------------------------------
template <typename T>
const T& getMax(const T& a, const T& b)
{
   return (a < b) ? b : a;
}

//---------------------------------------------------------------------------
//
//! \brief   get smaller value back
//
//! \author  Jo2003
//! \date    29.02.2012
//
//! \param   a first value
//! \param   b second value
//
//! \return  smaller value
//---------------------------------------------------------------------------
template <typename T>
const T& getMin(const T& a, const T& b)
{
   return (a < b) ? a : b;
}

#endif // __20120229_SMALL_HELPERS_H
