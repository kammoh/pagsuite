#!/bin/bash

#definitions:
#DEVICE="xc4vlx100-10ff1148";
#DEVICE="xc4vsx25-10ff668"; # Meyer-Baese 
#DEVICE="xc6vlx75t-2ff484"; #smalest v6 Konrad
DEVICE="xc6vsx475t-1ff1156" #v6, largest DSP device available
#DEVICE="xc6vlx550t-2ff1759" #big device
SPEED_GRADE="2" 
#DESIGN_GOAL="balanced"
#DESIGN_GOAL="speed"

if [ $# -lt 3 ]
  then
  echo "Usage: $0 (DESIGN GOAL: speed|balanced|speed_ucf) TLE (VHDL/Verilog FILENAME) [default project] [generics] [ip cores directory] [output filename]"
  exit 1
fi
DESIGN_GOAL=$1
TLE=$2;
FILE="$3"

#remove path:
FILE_WO_PATH=$(echo "./$FILE" | sed -n 's/.*\/\(.*\)/\1/p')

#get the extension .v or .vhd:
FILE_EXT=$(echo $FILE | sed -n 's/.*\.\(.*\)/\1/p')

if [ $FILE_EXT == "vhd" ]
then
  DESIGN_TYPE="vhdl"
  #remove extension .vhd:
  FILE_WO_EXT=$(echo $FILE_WO_PATH | sed -e s/.vhd//g)
else
	if [ $FILE_EXT == "vhdl" ]
	then
	  DESIGN_TYPE="vhdl"
	  #remove extension .vhd:
	  FILE_WO_EXT=$(echo $FILE_WO_PATH | sed -e s/.vhdl//g)
	else
	  DESIGN_TYPE="verilog"
	  #remove extension .v:
	  FILE_WO_EXT=$(echo $FILE_WO_PATH | sed -e s/.v//g)
	fi
fi


if [ $# -gt 4 ]
then
  GENERICS=$5;
else
  GENERICS=""
fi

if [ $# -gt 5 ]
then
  IPCORES_DIR="$6"
else
  IPCORES_DIR="ipcores"
fi

if [ $# -gt 6 ]
then
  #if default project file is given, cat it in the project file
  OUTPUT_FILENAME="$7"
else
  OUTPUT_FILENAME="$FILE_WO_EXT"
fi

#OUTPUT_DIR=results_$TLE\_$DESIGN_GOAL\_$DEVICE-$SPEED_GRADE
OUTPUT_DIR=results\_$DESIGN_GOAL\_$IPCORES_DIR\_$DEVICE-$SPEED_GRADE

#XST Options:
#Allows the flip-flop to be pulled into an IOB: -iob TRUE
#Use DSP Slice: -use_dsp48 AUTO|YES|NO

#MAP Options:
#Pack Registers in I/O: -pr off|i|o|b

#PAR Options:
#define router effort level: -rl std|high
#define placer effort level: -pl std|high

#Trace Options:
#verbose level 3: -v 3
#advanced analysis (incl. critical paths and max. frequency): -a
#Set speed grade to 10: -s 10
TRCE_ARGS="-a -v 3 "

case $DESIGN_GOAL in
"speed")
#XST options that equal to design goal "speed":
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 2 \
-move_first_stage NO \
-move_last_stage NO \
-iob Auto \
-register_duplication YES \
-register_balancing YES"

#SYNTHESIS_ARGS="-opt_mode Speed \
#-opt_level 2 \
#-move_first_stage NO \
#-move_last_stage NO \
#-iob TRUE \
#-register_duplication YES \
#-register_balancing YES"

#SYNTHESIS_ARGS="-ofmt NGC \
#-opt_mode Speed \
#-opt_level 1 \
#-power NO \
#-iuc NO \
#-keep_hierarchy No \
#-netlist_hierarchy As_Optimized \
#-rtlview Yes \
#-glob_opt AllClockNets \
#-read_cores YES \
#-write_timing_constraints NO \
#-cross_clock_analysis NO \
#-hierarchy_separator / \
#-bus_delimiter <> \
#-case Maintain \
#-slice_utilization_ratio 100 \
#-bram_utilization_ratio 100 \
#-dsp_utilization_ratio 100 \
#-lc Auto \
#-reduce_control_sets Auto \
#-fsm_extract YES -fsm_encoding Auto \
#-safe_implementation No \
#-fsm_style LUT \
#-ram_extract Yes \
#-ram_style Auto \
#-rom_extract Yes \
#-shreg_extract YES \
#-rom_style Auto \
#-auto_bram_packing NO \
#-resource_sharing YES \
#-async_to_sync NO \
#-shreg_min_size 2 \
#-use_dsp48 Auto \
#-iobuf YES \
#-max_fanout 100000 \
#-bufg 32 \
#-register_duplication YES \
#-register_balancing Yes \
#-move_first_stage NO \
#-move_last_stage NO \
#-optimize_primitives NO \
#-use_clock_enable Auto \
#-use_sync_set Auto \
#-use_sync_reset Auto \
#-iob Auto \
#-equivalent_register_removal YES \
#-slice_utilization_ratio_maxmargin 5"

# ISE: -ir off -pr off
# Script: -detail -timing -xe n -pr b
# -mt 2: Use 2 threads
# -pr off/b: pack registers in I/O

#MAP_ARGS="-detail \
#-timing \
#-logic_opt off \
#-ol high \
#-xe n \
#-t 1 \
#-mt 2 \
#-register_duplication off \
#-global_opt off \
#-pr off \
#-power off"

MAP_ARGS="-detail \
-timing \
-logic_opt off \
-ol high \
-t 1 \
-mt 2 \
-register_duplication off \
-global_opt off \
-pr off \
-power off"

#MAP_ARGS="-w -logic_opt off -ol high -t 1 -xt 0 -register_duplication off -r 4 -global_opt off -mt off -ir off -pr off -lc off -power off"

NGDBUILD_ARGS=""

PAR_ARGS="-ol high";;

"speed_no_ret_no_dup")
#speed optimization without register duplication and without retiming (register balancing):
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 2 \
-move_first_stage NO \
-move_last_stage NO \
-iob Auto \
-register_duplication NO \
-register_balancing NO"

MAP_ARGS="-detail \
-timing \
-logic_opt off \
-ol high \
-t 1 \
-mt 2 \
-register_duplication off \
-global_opt off \
-pr off \
-power off"

NGDBUILD_ARGS=""

PAR_ARGS="-ol high";;
"speed_no_ret_no_dup_r8")
#speed optimization without register duplication and without retiming (register balancing) and forcing the use of 8 FF per slice:
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 2 \
-move_first_stage NO \
-move_last_stage NO \
-iob Auto \
-register_duplication NO \
-register_balancing NO"

MAP_ARGS="-detail \
-timing \
-logic_opt off \
-ol high \
-t 1 \
-mt 2 \
-r 8 \
-register_duplication off \
-global_opt off \
-pr off \
-power off"

NGDBUILD_ARGS=""

PAR_ARGS="-ol high";;
"speed_no_ret_no_dup_iob")
#speed optimization without register duplication and without retiming (register balancing):
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 2 \
-move_first_stage NO \
-move_last_stage NO \
-iob TRUE \
-register_duplication NO \
-register_balancing NO"

MAP_ARGS="-detail \
-timing \
-logic_opt off \
-ol high \
-t 1 \
-mt 2 \
-register_duplication off \
-global_opt off \
-pr off \
-power off"

NGDBUILD_ARGS=""

PAR_ARGS="-ol high";;

"speed_no_ret_no_dup_area")
#speed optimization without register duplication and without retiming (register balancing) and area aggressive mapping:
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 2 \
-move_first_stage NO \
-move_last_stage NO \
-iob Auto \
-register_duplication NO \
-register_balancing NO"

#-c 1 is used for aggressive area minimization and maximum logic density
#-lc area is used for combining LUT5 into LUT6
MAP_ARGS="-detail \
-timing \
-c 1 \
-lc area \
-logic_opt off \
-ol high \
-t 1 \
-mt 2 \
-register_duplication off \
-global_opt off \
-pr off \
-power off"
#-ir off
NGDBUILD_ARGS=""

PAR_ARGS="-ol high";;

"speed_no_ret_no_dup_area_iob")
#speed optimization without register duplication and without retiming (register balancing) and area aggressive mapping with FF in IOB:
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 2 \
-move_first_stage NO \
-move_last_stage NO \
-iob TRUE \
-register_duplication NO \
-register_balancing NO"

#-c 1 is used for aggressive area minimization and maximum logic density
#-lc area is used for combining LUT5 into LUT6
MAP_ARGS="-detail \
-timing \
-c 1 \
-lc area \
-logic_opt off \
-ol high \
-t 1 \
-mt 2 \
-register_duplication off \
-global_opt off \
-pr off \
-power off"
#-ir off
NGDBUILD_ARGS=""

PAR_ARGS="-ol high";;

"speed_no_ret_no_dup_area50") 
#speed optimization without register duplication and without retiming (register balancing):
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 2 \
-move_first_stage NO \
-move_last_stage NO \
-iob Auto \
-register_duplication NO \
-register_balancing NO"

#-c 1 is used for aggressive area minimization and maximum logic density
#-lc area is used for combining LUT5 into LUT6
MAP_ARGS="-detail \
-timing \
-c 50 \
-lc area \
-logic_opt off \
-ol high \
-t 1 \
-mt 2 \
-register_duplication off \
-global_opt off \
-pr off \
-power off"
#-ir off
NGDBUILD_ARGS=""

PAR_ARGS="-ol high";;

"speed_no_ret_no_dup_ucf")
#speed optimization without register duplication and without retiming (register balancing):
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 2 \
-move_first_stage NO \
-move_last_stage NO \
-iob Auto \
-register_duplication NO \
-register_balancing NO"

MAP_ARGS="-detail \
-timing \
-logic_opt off \
-ol high \
-t 1 \
-mt 2 \
-register_duplication off \
-global_opt off \
-pr off \
-power off"

NGDBUILD_ARGS="-uc constraints.ucf"

PAR_ARGS="-ol high";;

"speed_no_ret_no_dup_keep_hierarchy")
#speed optimization without register duplication and without retiming (register balancing) but with keep hierarchy:
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 2 \
-move_first_stage NO \
-move_last_stage NO \
-iob Auto \
-keep_hierarchy YES \
-register_duplication NO \
-register_balancing NO"

MAP_ARGS="-detail \
-timing \
-logic_opt off \
-ol high \
-t 1 \
-mt 2 \
-register_duplication off \
-global_opt off \
-pr off \
-power off"

NGDBUILD_ARGS=""

PAR_ARGS="-ol high";;

"speed_no_dup")
#speed optimization without register duplication:
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 2 \
-move_first_stage NO \
-move_last_stage NO \
-iob Auto \
-register_duplication NO \
-register_balancing YES"

MAP_ARGS="-detail \
-timing \
-logic_opt off \
-ol high \
-t 1 \
-mt 2 \
-register_duplication off \
-global_opt off \
-pr off \
-power off"

NGDBUILD_ARGS=""

PAR_ARGS="-ol high";;

"speed_no_ret_no_dup_no_iob_no_reg_rem")
#speed optimization without register duplication and without retiming (register balancing):
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 2 \
-move_first_stage NO \
-move_last_stage NO \
-iob FALSE \
-equivalent_register_removal NO \
-register_duplication NO \
-register_balancing NO"

MAP_ARGS="-detail \
-timing \
-logic_opt off \
-ol high \
-t 1 \
-mt 2 \
-register_duplication off \
-global_opt off \
-pr off \
-power off"

NGDBUILD_ARGS=""

PAR_ARGS="-ol high";;


"speed_ucf")
#XST options that equal to design goal "speed":
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 2 \
-move_first_stage NO \
-move_last_stage NO \
-iob TRUE \
-register_duplication YES \
-register_balancing YES"

MAP_ARGS="-detail \
-timing \
-logic_opt off \
-ol high \
-xe n \
-t 1 \
-register_duplication off \
-global_opt off \
-pr b \
-power off"

NGDBUILD_ARGS="-uc constraints.ucf"

PAR_ARGS="-ol high";;

"speed_wo_shiftreg_extract")
#XST options that equal to design goal "speed":
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 2 \
-move_first_stage NO \
-move_last_stage NO \
-iob TRUE \
-register_duplication YES \
-shreg_extract no \
-register_balancing YES"

NGDBUILD_ARGS=""

MAP_ARGS="-detail \
-timing \
-logic_opt off \
-ol high \
-xe n \
-t 1 \
-register_duplication on \
-retiming on \
-global_opt speed \
-pr b \
-power off"

PAR_ARGS="-ol high";;


"speed_no_ret_no_dup_no_shiftreg_extract")
#like speed_no_ret_no_dup but also without use of shift register chains
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 2 \
-move_first_stage NO \
-move_last_stage NO \
-iob Auto \
-shreg_extract no \
-register_duplication NO \
-register_balancing NO"

MAP_ARGS="-detail \
-timing \
-logic_opt off \
-ol high \
-t 1 \
-mt 2 \
-register_duplication off \
-global_opt off \
-pr off \
-power off"

NGDBUILD_ARGS=""

PAR_ARGS="-ol high";;



"balanced")
#XST options that equal to design goal "balanced" (default ISE project settings):
SYNTHESIS_ARGS="-opt_mode Speed \
-opt_level 1  \
-move_first_stage NO \
-move_last_stage NO \
-iob AUTO \
-register_duplication YES \
-register_balancing No"

NGDBUILD_ARGS=""

MAP_ARGS="-pr off -detail"


PAR_ARGS="";;


"area")
#area optimization:
SYNTHESIS_ARGS="-opt_mode area \
-opt_level 2  \
-move_first_stage NO \
-move_last_stage NO \
-iob AUTO \
-register_duplication NO \
-register_balancing NO"

NGDBUILD_ARGS=""

#-timing \

MAP_ARGS="-detail \
-logic_opt off \
-ol high \
-t 1 \
-mt 2 \
-register_duplication off \
-global_opt area \
-lc area \
-pr off \
-power off"

PAR_ARGS="-ol high";;



*)
  echo "invalid design goal: $DESIGN_GOAL"
  exit -1
esac

if [ $# -gt 3 ]
then
  #if default project file is given, cat it in the project file
  cat $4 > $TLE.prj
  echo -e "\n$DESIGN_TYPE work \"$FILE\"\n" >> $TLE.prj
else
  echo -e "\n$DESIGN_TYPE work \"$FILE\"\n" > $TLE.prj
fi

TIME=$(date)

echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo "+++ Starting synthesis of $TLE using $FILE with design goal $DESIGN_GOAL, at $TIME,"
echo "+++ storing results to $OUTPUT_DIR +++++++++++++++"
echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

echo ">>> run -ifn $TLE.prj -top $TLE -ofn $TLE -p $DEVICE $SYNTHESIS_ARGS -generics {$GENERICS}"
echo "run -ifn $TLE.prj -top $TLE -ofn $TLE -p $DEVICE $SYNTHESIS_ARGS -generics {$GENERICS}" | xst | tee $TLE.xst
if [ "$?" = "0" ]
then
  echo "+++++++++++++++ Starting Translate +++++++++++++++"
  ngdbuild -p $DEVICE $NGDBUILD_ARGS -sd ../ipcores/$IPCORES_DIR $TLE.ngc
  if [ "$?" = "0" ]
  then
    echo "+++++++++++++++ Starting Mapping +++++++++++++++"
    map -p $DEVICE $MAP_ARGS $TLE.ngd
    if [ "$?" = "0" ]
    then
      echo "+++++++++++++++ Starting Place&Route +++++++++++++++"
      par $PAR_ARGS -w $TLE.ncd parout.ncd $TLE.pcf
      if [ "$?" = "0" ]
      then
#      echo "+++++++++++++++ Converting NCD files to XST +++++++++++++++"
#      xdl -ncd2xdl $TLE.ncd
#      xdl -ncd2xdl parout.ncd
        echo "+++++++++++++++ Starting Static-Timing Analysis +++++++++++++++"
        trce $TRCE_ARGS  -xml $TLE.twx -o $TLE.twr parout.ncd $TLE.pcf
        #if not existing, create output directory:
        if [ ! -d $OUTPUT_DIR ] 
        then
          mkdir $OUTPUT_DIR
        fi
        echo  "+++++++++++++++ moving result files +++++++++++++++"
        mv $TLE.mrp $OUTPUT_DIR/$OUTPUT_FILENAME.mrp
        mv $TLE.xst $OUTPUT_DIR/$OUTPUT_FILENAME.xst
        mv $TLE.twr $OUTPUT_DIR/$OUTPUT_FILENAME.twr
        mv $TLE.twx $OUTPUT_DIR/$OUTPUT_FILENAME.twx
        mv $TLE\_usage.xml $OUTPUT_DIR/$OUTPUT_FILENAME\_usage.xml
        mv $TLE.ncd $OUTPUT_DIR/$OUTPUT_FILENAME.ncd
        mv parout.ncd $OUTPUT_DIR/$OUTPUT_FILENAME\_par.ncd
#        mv $TLE.xdl $OUTPUT_DIR/$OUTPUT_FILENAME.xdl
#        mv parout.xdl $OUTPUT_DIR/$OUTPUT_FILENAME\_par.xdl
        mv $TLE.pcf $OUTPUT_DIR/$OUTPUT_FILENAME.pcf
        echo $TLE\_usage.xml $OUTPUT_DIR/$OUTPUT_FILENAME\_usage.xml
      else
        echo "+++++++++++++++ Skipping Static-Timing Analysis due to previous errors +++++++++++++++"
      fi  
    else
      echo "+++++++++++++++ Skipping Place&Route due to previous errors +++++++++++++++"
    fi  
  else
    echo "+++++++++++++++ Skipping Mapping due to previous errors +++++++++++++++"
  fi  
else
  echo "+++++++++++++++ Skipping Translate due to previous errors +++++++++++++++"
fi  

#source ../scripts/clean.sh
