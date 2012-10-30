#!/bin/sh
PACKAGE_PATH=`pwd 2>&1`
export CLASSPATH=$PACKAGE_PATH/as-distances-1.0/se/sics/asdistances:.
cd as-distances-1.0
javac -d . ASHop.java
