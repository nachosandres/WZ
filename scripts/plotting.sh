#!/bin/bash

VAR=("njets" "nbjets" "met" "ht" "pt1" "pt2" "pt3" )


for var in ${VAR[@]}; do

    variable=$var

    cp cards/susy3l_tt_closure.C cards/tmpFiles/susy3l_tt_closure.C
    sed -i 's|VARIABLE|'$variable'|' cards/tmpFiles/susy3l_tt_closure.C

    root -l -b cards/tmpFiles/susy3l_tt_closure.C

done

