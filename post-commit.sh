#!/bin/sh

echo "doing post-commit hook"

if [ $(git symbolic-ref --short HEAD) == "master" ]
then
  echo "on master, tagging"
  tag_name=`python -c "print('v'+'.'.join(open('VERSION').readlines()[0].split()))"`
  git tag -a $tag_name -m "automatically tagging via post-commit hook"
  echo "tag is $tag_name"
else
  echo "not on master, not bumping"
fi
