/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/tables.h $
|
| Author: Jo2003
|
| Begin: 13.06.2010 / 14:50:35
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: tables.h 1254 2013-12-03 08:35:13Z Olenka.Joerg $
\*************************************************************/
#ifndef __130610_TABLES_H
   #define __130610_TABLES_H

#define TAB_ASPECT \
"CREATE TABLE aspect ("\
      "cid integer PRIMARY KEY UNIQUE,"\
      "asp char(32),"\
      "crop char(32))"

#define TAB_SETTINGS \
"CREATE TABLE settings ("\
      "name char(32) PRIMARY KEY UNIQUE,"\
      "val text)"

#define TAB_TIMERREC \
"CREATE TABLE timerrec ("\
      "id integer PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,"\
      "cid integer NOT NULL,"\
      "timeshift  integer,"\
      "recstart unsigned int(10) NOT NULL,"\
      "recend unsigned int(10) NOT NULL,"\
      "name tiny text NOT NULL)"

#define TAB_SHORTCUTS \
"CREATE TABLE shortcuts ("\
      "target VARCHAR(50) NOT NULL,"\
      "slot VARCHAR(100) NOT NULL,"\
      "key_sequence VARCHAR(25) NULL DEFAULT NULL,"\
      "PRIMARY KEY (target, slot))"

#define TAB_WATCHLIST \
"CREATE TABLE watchlist ("\
      "cid integer NOT NULL," \
      "t_start unsigned int(10) NOT NULL,"\
      "t_end unsigned int(10) NOT NULL,"\
      "t_eol unsigned int(10) NOT NULL,"\
      "name VARCHAR(50) NOT NULL,"\
      "prog tiny text NOT NULL," \
      "PRIMARY KEY (cid, t_start))"

#define TAB_ASTREAM \
"CREATE TABLE \"astream\" ("\
      "\"cid\"  INTEGER NOT NULL,"\
      "\"aidx\"  INTEGER NOT NULL,"\
      "\"code\"  VARCHAR(10) NOT NULL,"\
      "PRIMARY KEY(\"cid\"),"\
      "UNIQUE(\"cid\"))"

#define TAB_FAVDATA \
"CREATE TABLE \"favdata\" ("\
      "\"cid\"  INTEGER NOT NULL,"\
      "\"gid\"  INTEGER NOT NULL,"\
      "\"name\"  VARCHAR(100) NOT NULL,"\
      "\"logo\"  VARCHAR(100) NOT NULL,"\
      "PRIMARY KEY(\"cid\"),"\
      "UNIQUE(\"cid\"))"

#endif // __130610_TABLES_H
/************************* History ***************************\
| $Log$
\*************************************************************/
