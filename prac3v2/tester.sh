#!/bin/bash

PORT=40002

cd wojciech_fica
make

{ /usr/bin/time -v ./transport $PORT out $1 > /dev/null ; } 2> ../my_time 
mv out ../
cd ../
echo "my stat"
grep "Elapsed" my_time
grep "Maximum resident set size" my_time

{ /usr/bin/time -v ./transport-faster $PORT out_ref $1 > /dev/null ; } 2> mbi_time


echo "transport-faster stat"
grep "Elapsed" mbi_time
grep "Maximum resident set size" mbi_time

diff --brief out out_ref
rm out out_ref mbi_time my_time