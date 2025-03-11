#!/bin/sh

update_nsh()
{
    lang=`echo $2 | cut -d . -f 1 | sed 's/\(.\)/\u\1/'`

    echo $1 $2 $lang
    sed s/LANG_English/LANG_${lang}/ english.nsh | sed 's/".*"//; s/[ \t\r]*$//' > $2
    dos2unix $2

    sed 's/.*/"&"/' $1 > $1.tmp
    paste -d " " $2 $1.tmp > $2.tmp
    mv $2.tmp $2
    unix2dos $2
    rm $1.tmp
}

update_nsh fi.txt finnish.nsh
update_nsh it.txt italian.nsh
update_nsh ko.txt korean.nsh
update_nsh nl.txt dutch.nsh
update_nsh pl_PL.txt polish.nsh
update_nsh ru.txt russian.nsh
