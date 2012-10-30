#!/bin/sh
PACKAGE_PATH=`pwd 2>&1`
export CLASSPATH=$PACKAGE_PATH/as-distances-1.0/se/sics/asdistances:.
cd $PACKAGE_PATH/as-distances-1.0
AS_HOP="$(java se.sics.asdistances.ASHop $1 $2)"
echo $AS_HOP

