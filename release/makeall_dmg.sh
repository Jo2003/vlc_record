#!/bin/bash
ALL=`ls -d *`

for i in $ALL ; do
    if [ -d $i ] ; then
        echo Processing $i ...
        ./create_dmg.sh $i
    fi
done

