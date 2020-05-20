#!/usr/bin/bash


d=`date +"%Y%m%d"`

run(){
    pid=`expr $1 + 1`
    pname=$2
    echo "$pid $pname"
    if [[ $# -lt 2 ]]; then
        echo "you cmd mabe like run.sh 1 yule"
        exit
    fi

    rm -f data/$pname/$d/*
    python ExportArticle.py -p $pid -d $pname
    if [[ $? -eq 0 ]]; then
        python KeywordsExtract.py -d $pname
    fi
    if [[ $? -eq 0 ]]; then
        python RecommedationYule.py -d $pname
    fi
    if [[ $? -eq 0 ]]; then
        python SaveGenreList.py -p $pid -d $pname &
    fi
}

project=("yule" "jiaoyu" "youxi")
for p in ${!project[*]}; do
    run $p ${project[$p]}
done
exit
