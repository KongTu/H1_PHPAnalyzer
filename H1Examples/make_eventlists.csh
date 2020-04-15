#!/bin/csh -f


set evtlistdir="/h1/root/checkout/oo-releases/eventlists"
set rel="4.0"
set release="4.0.4"
set dstvers=7
set year=04
set acs=acs

#foreach i ( 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 ) #06 
#foreach i ( 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 ) # 97 
#foreach i ( 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 ) # 99
#foreach i ( 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 ) #05
#foreach i ( 00 01 02 03 04 05 06 07 08 09 10 11 12 13 ) # 05 dst3
#foreach i ( 00 01 02 03 04 05 06 07 08 09 10 11 12 ) # 96
#foreach i ( 00 01 02 03 04 05 06 07 08 09 ) # 07 high energy data
#foreach i ( 00 01 ) # 98 / 03 
#foreach i ( 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 ) # 07 low/middle energy runs
#foreach i ( 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 ) # 00
#foreach i ( 17 ) # 00 shifted vertex run
foreach i ( 00 01 02 03 04 05 ) # 04

set steerfile=write_eventlist_${acs}_${year}_${i}.steer
cat >! $steerfile << EOF
H1SteerTree() {
  fHatFiles  = "/${acs}/data/oo-${rel}/${year}/hat.${release}.10dst${dstvers}.c${year}0${i}0-${i}9.root";
  fModsFiles = "/${acs}/data/oo-${rel}/${year}/mods.${release}.10dst${dstvers}.c${year}0${i}0-${i}9.root";
}
EOF

write_eventlist -o ${evtlistdir}/list.${release}.dst${dstvers}.${year}.${acs}.d$i.root -f write_eventlist_${acs}_${year}_$i.steer

# rm $steerfile
end
