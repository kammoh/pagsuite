#!/bin/bash

if [ ! $# -eq 2 ]; then
	echo "usage: simulate INSTANCE_NAME FILE_NAME"
	exit -1
fi

INSTANCE_NAME=$1
FILE_NAME=$2

export XILINX=/opt/xilinx/ise13.4/ISE_DS/ISE
export PLATFORM=lin64

if [[ "$LD_LIBRARY_PATH" =~ "/opt/Xilinx/13.4/ISE_DS/ISE/lib/lin64" ]]; then
	export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/opt/Xilinx/13.4/ISE_DS/ISE/lib/lin64"
fi

if [ -d ${INSTANCE_NAME}_sim.d ]; then
	rm -rf ${INSTANCE_NAME}_sim.d
fi

mkdir ${INSTANCE_NAME}_sim.d
cd ${INSTANCE_NAME}_sim.d
cp ../$FILE_NAME $FILE_NAME

echo "vhdl work "$FILE_NAME > $INSTANCE_NAME.prj
echo ">Compiling..."
fuse $INSTANCE_NAME -incremental -o $INSTANCE_NAME.simu -prj $INSTANCE_NAME.prj > /dev/null 2>&1

if [ $? -eq 0 ]; then
	echo "#!/usr/bin/tclsh" > $INSTANCE_NAME.tcl
	echo "run all" >> $INSTANCE_NAME.tcl
	echo "quit" >> $INSTANCE_NAME.tcl
	echo ">Simulating..."
	./$INSTANCE_NAME.simu -tclbatch $INSTANCE_NAME.tcl > /dev/null 2>&1	
	cp fuse.log ../${INSTANCE_NAME}_fuse.log
	cp isim.log ../${INSTANCE_NAME}_isim.log
	
	NO_OF_ERRORS=`cat isim.log | grep "Error:" | wc -l`
	echo ">Simulation exited with $NO_OF_ERRORS errors"	
	cd ..
	rm -rf ${INSTANCE_NAME}_sim.d
	if [ $NO_OF_ERRORS -gt 0 ]; then
		exit -1
	fi
else
	cp fuse.log ../${INSTANCE_NAME}_fuse.log
	
	
	
	rm -rf ${INSTANCE_NAME}_sim.d
	echo ">Failed to compile simulation"
	cat fuse.log | grep "ERROR:HDLCompiler"
	exit -1
fi

exit 0