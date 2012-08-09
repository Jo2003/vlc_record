/*
 * Copyright (C) 2008 Remko Troncon
 */

#include "CocoaInitializer.h"

#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
#include <QtDebug>

class CocoaInitializer::Private
{
   public:
      NSAutoreleasePool* _autoReleasePool;
};

CocoaInitializer::CocoaInitializer()
{
   d = new CocoaInitializer::Private();
   NSApplicationLoad();
   d->_autoReleasePool = [[NSAutoreleasePool alloc] init];
}

CocoaInitializer::~CocoaInitializer()
{
   [d->_autoReleasePool release];
   delete d;
}
