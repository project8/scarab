#!/bin/sh

TOP=$(pwd)

SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

ln -s $SCRIPTDIR/post-merge.sh $TOP/.git/hooks/post-merge
ln -s $SCRIPTDIR/pre-commit.sh $TOP/.git/hooks/pre-commit
ln -s $SCRIPTDIR/bump_version.py $TOP/.git/hooks/bump_version.py
