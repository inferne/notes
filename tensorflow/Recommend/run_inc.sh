#!/usr/bin/bash

pid=$1
pname=$2
echo "$pid $pname"
if [[ $# -lt 2 ]]; then
    echo "you cmd mabe like run.sh 1 yule"
    exit
fi
python ExportArticleInc.py -p $pid -d $pname
if [[ $? -eq 0 ]]; then
    python KeywordsExtractInc.py -d $pname
fi
if [[ $? -eq 0 ]]; then
    python RecommedationInc.py -d $pname
fi
if [[ $? -eq 0 ]]; then
    python SaveGenreListInc.py -p $pid -d $pname
fi
