#!/bin/sh

dir=$(pwd)

ln -s $dir/post-merge.sh .git/hooks/post-merge
ln -s $dir/pre-commit.sh .git/hooks/pre-commit
