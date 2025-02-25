#!/bin/bash
# replace string in src subdir
for entry in src/*/*.* ; do
  if grep -q -Fwi $1 $entry ; then
    echo "replace $1 in $entry"
    sed -i "s@$1@$2@g" $entry
  fi
done
