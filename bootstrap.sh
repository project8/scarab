#!/bin/sh

dir=$(pwd)

SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

ln -s $SCRIPTDIR/post-merge.sh $dir/.git/hooks/post-merge
ln -s $SCRIPTDIR/pre-commit.sh $dir/.git/hooks/pre-commit
