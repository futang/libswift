#!/bin/sh
cd ../ext
PACKAGE_PATH=`pwd 2>&1`
echo $PACKAGE_PATH
export CLASSPATH=$PACKAGE_PATH/as-distances-1.0/se/sics/asdistances:.
cd $PACKAGE_PATH/as-distances-1.0
javac -d . ASHop.java
