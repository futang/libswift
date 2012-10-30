#!/bin/sh
PACKAGE_PATH=`pwd 2>&1`
export CLASSPATH=../ext/as-distances-1.0/se/sics/asdistances:.
cd ../ext/as-distances-1.0
javac -d . ASHop.java
