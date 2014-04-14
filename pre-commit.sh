#!/bin/sh

echo "doing pre-commit hook"

if [ $(git symbolic-ref --short HEAD) == "master" ]
then
  echo "on master, bumping"
  python bump_version.py
  git add VERSION
else
  echo "not on master, not bumping"
fi
