#!/bin/sh

update_nsh()
{
    lang=`echo $2 | cut -d . -f 1 | sed 's/\(.\)/\u\1/'`

    echo $1 $2 $lang
    sed s/LANG_English/LANG_${lang}/ English.nsh | sed 's/".*"//; s/[ \t\r]*$//' > $2
    dos2unix $2

    sed 's/.*/"&"/' $1 > $1.tmp
    paste -d " " $2 $1.tmp > $2.tmp
    mv $2.tmp $2
    unix2dos $2
    rm $1.tmp
}

update_nsh fi.txt Finnish.nsh
update_nsh fr.txt French.nsh
update_nsh it.txt Italian.nsh
update_nsh ko.txt Korean.nsh
update_nsh nl.txt Dutch.nsh
update_nsh pl_PL.txt Polish.nsh
update_nsh ru.txt Russian.nsh
update_nsh zh_TW.txt TradChinese.nsh
