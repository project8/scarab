#!/bin/sh

echo "Doing post merge"

bash pre-commit.sh

echo `pwd`

echo $@
