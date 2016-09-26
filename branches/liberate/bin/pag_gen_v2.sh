#!/bin/bash

check_failure(){
	if [ ! $? -eq 0 ]; then
		failure $1 $2
	fi
}

failure(){
	COMPONENT=$1
	MESSAGE=$2
	echo "[${COMPONENT}] $MESSAGE" 1>&2
	dot -Tpdf -O *.dot >/dev/null 2>&1
	exit -1;
}

print_help(){
	echo -e "\033[1mUsage:\033[0m pag_gen_v2.sh [OPTIONS] [\"COEFFICIENTS_LIST\"]"
	echo -e "\033[1mCOEFFICIENT_LIST:\033[0m"
	echo -e "  List of coefficients to be computed, not needed when \033[1m-b\033[0m is set"
	echo -e "  but then instance with name specified by \033[1m-n\033[0m has to exist."
	echo -e "  Configurations are seperated by ';' and inputs by ','"
	echo -e "  \033[1mCaution:\033[0m Use \"COEFFICIENT_LIST\",otherwise bash will take ';' as end of line"
	echo -e " "
	echo -e "\033[1mOPTIONS:\033[0m"
	echo -e "  \033[1m-t, --ternary\033[0m"
	echo -e "			enables the usage of ternary adders"
	echo -e "  \033[1m-c, --clean\033[0m"
	echo -e "			cleans the output directory before starting"
	echo -e "  \033[1m-a, --auto\033[0m"
	echo -e "			begins with the first job not completed in last compution"
	echo -e "  \033[1m-n INSTANCENAME, --name INSTANCENAME\033[0m"
	echo -e "			sets the instancename for outputs," 
	echo -e "			also names output directory; default: 'TMP'"
	echo -e "  \033[1m-o DIR, --output-dir DIR\033[0m"
	echo -e "			sets the output directory, if not exist will be created" 
	echo -e "			outputs will be generated in DIR/INSTANCENAME/"
	echo -e "  \033[1m--copy-suffix SUFF\033[0m"
	echo -e "			copies the instance folder to new folder with suffix"
	echo -e "			only valid for existing instances"
	echo -e "  \033[1m-b JOBNAME, --begin JOBNAME\033[0m"
	echo -e "			sets the jobname to start with, INSTANCENAME has to be given,"
	echo -e "			directory must exist and previous steps has to be completed;"
	echo -e "			valid are (rpag|split|fusion|flopoco|synth)"
	echo -e "  \033[1m-v [INPUT_WS], --vhdl [INPUT_WS]\033[0m"
	echo -e "			generates vhdl code when graph is build and optional sets "
	echo -e "			input wordsize for component; default: INPUT_WS=10"
	echo -e "  \033[1m-s [SIM_SIZE], --sim [SIM_SIZE]\033[0m"
	echo -e "			adds simulation testbench to generated vhdl code and sets"
	echo -e "			number of random values to be tested; "
	echo -e "			implies \033[1m-v\033[0m; for changing wordsize set option \033[1m-v\033[0m explicitly"
	echo -e "			default: SIM_SIZE=100"
	echo -e "  \033[1m-x, --synth\033[0m"
	echo -e "			synthesize generated vhdl code;"
	echo -e "			implies \033[1m-v\033[0m; for changing wordsize set option \033[1m-v\033[0m explicitly"
	echo -e "  \033[1m--rpag ARGS\033[0m"
	echo -e "			specify additional arguments for rpag call; caution: use \"ARGS\""
	echo -e "  \033[1m--fusion ARGS\033[0m"
	echo -e "			specify additional arguments for pag_fusion call; caution: use \"ARGS\""
	echo -e "  \033[1m--flopoco ARGS\033[0m"
	echo -e "			specify additional arguments for flopoco call; caution: use \"ARGS\""
	echo -e "  \033[1m--flopoco-sync-opt ARGS\033[0m"
	echo -e "			specify additional sync options for flopoco call; caution: use \"ARGS\""
	echo -e "  \033[1m-h, --help\033[0m"
	echo -e "			print this help"
	echo -e "\033[1mENVIRONMENT:\033[0m"
	echo -e "  \033[1mPAG_GEN_BINARIES\033[0m"
	echo -e "			set if binaries are located in different directory"
	echo -e "\033[1mEXAMPLES:\033[0m"
	echo -e "  pag_gen_v2.sh -n NAME \"123\""
	echo -e "  pag_gen_v2.sh -n NAME \"123;321\""
	echo -e "  pag_gen_v2.sh -n NAME -atx \"123,321;321,-123\""
}

parse_coeff_list() {
	OUT_COUNT=0
	CFG_COUNT=0
	INPUT_COUNT=0
	TMP_LIST=
	for x in $COEFFICIENT_LIST
	do
		if [ $OUT_COUNT -gt 0 ]; then
			TMP_LIST="${TMP_LIST} "
		fi
		CFGS=$(echo $x | tr ";" " ")
		((OUT_COUNT=$OUT_COUNT+1))
		CUR_CFG_COUNT=0
		for y in $CFGS
		do
			if [ $CUR_CFG_COUNT -gt 0 ]; then
					TMP_LIST="${TMP_LIST};"
			fi
			((CUR_CFG_COUNT=$CUR_CFG_COUNT+1))
			CUR_INP_COUNT=0
			INS=$(echo $y | tr "," " ")
			for z in $INS
			do
				if [ $CUR_INP_COUNT -gt 0 ]; then
					TMP_LIST="${TMP_LIST},"
				fi
				((CUR_INP_COUNT=$CUR_INP_COUNT+1))
				if ! [ "$z" -eq "$z" ] 2>/dev/null; then
				  	failure "COEFF_LIST" "Not a number: "$z
					exit -1
				fi
				
				TMP_LIST="${TMP_LIST}$z"
				#TMP_LIST="${TMP_LIST}${z#-}" ursprüngliche Version
			done
			if [ $INPUT_COUNT -eq 0 ]; then
				INPUT_COUNT=${CUR_INP_COUNT}
			elif [ $INPUT_COUNT != ${CUR_INP_COUNT} ]; then
				failure "COEFF_LIST" "Input count missmatch: "$y
				exit -1
			fi
		done
		if [ $CFG_COUNT -eq 0 ]; then
			CFG_COUNT=${CUR_CFG_COUNT}
		elif [ $CFG_COUNT != ${CUR_CFG_COUNT} ]; then
			failure "COEFF_LIST" "Configuration count missmatch: "$x
			exit -1
		fi
	done
	COEFFICIENT_LIST=$TMP_LIST
}

SYNTH_SCRIPT_NAME="synthesize_xilinx_v6.sh"
RPAG_ARGS="$RPAG_ARGS --file_output=true"
FLOPOCO_SYNC_OPT="addregevery1"

BEGIN_WITH=rpag
INSTANCE_NAME="TMP"
TERNARY_MODE=false
PRINT_DISABLE=false
CLEAN=false
GENERATE_VHDL=false
VHDL_INPUT_WORDSIZE=10
GENERATE_SIM=false
SIM_SIZE=100
SYNTHESIZE=false
OUTPUT_DIR=$PWD
AUTOMATIC_MODE=false
RUN_RPAG_TOXED=false
COPY_SUFFIX=

if [ $# -eq 0 ]; then
	print_help
	exit 0
fi

TEMP="$(getopt --name 'pag_gen_v2.sh' --options htcv::s::xn:b:o:a --long help,ternary,clean,begin:,vhdl::,sim::,synth,name:,rpag:,fusion:,flopoco:,output-dir:,flopoco-sync-opt:,auto,toxed,copy-suffix: -- $@)"
if ! [ $? -eq 0 ]; then
	failure "GETOPT" "Bad option";
fi
echo "$TEMP"
eval set -- "$TEMP"

while true; do
	case $1 in
	--auto|-a)		AUTOMATIC_MODE=true; shift;;
	--ternary|-t) 	TERNARY_MODE=true; shift;;
	--clean|-c)		CLEAN=true;	shift;;
	--toxed)		RUN_RPAG_TOXED=true; shift;;
	--begin|-b) 	if [[ "$2" =~ ^(rpag|split|fusion|flopoco|synth|print)$ ]];then BEGIN_WITH=$2; else failure "CMD_PARSE" "Beginning step not recognized"; fi; shift 2;;
	--vhdl|-v) 		GENERATE_VHDL=true; if [ ${#2} -gt 0 ]; then VHDL_INPUT_WORDSIZE=$2; fi; shift 2;;
	--sim|-s) 		GENERATE_VHDL=true; GENERATE_SIM=true; if [ ${#2} -gt 0 ]; then SIM_SIZE=$2; fi; shift 2;;
	--output-dir|-o) 
					OUTPUT_DIR=$2; shift 2;;
	--copy-suffix)  COPY_SUFFIX=$2; shift 2;;
	--synth|-x) 	GENERATE_VHDL=true; SYNTHESIZE=true; shift;;
	--name|-n) 		INSTANCE_NAME=$2; shift 2;;
	--help|-h)		print_help; exit 0;;
	--fusion)		FUSION_ARGS="$FUSION_ARGS $2"; shift 2;;
	--rpag)			RPAG_ARGS="$RPAG_ARGS $2"; shift 2;;
	--flopoco)		FLOPOCO_ARGS="$FLOPOCO_ARGS $2"; shift 2;;
	--flopoco-sync-opt)	
					FLOPOCO_SYNC_OPT="$2"; shift 2;;
#	--bin-path)		set_bin_path $2; exit 0;;
	--) 			shift; break;;
	*) 				failure "CMD_PARSE" "Unrecognized option"; exit 1;;
	esac
done

# READ AND CHECK COEFFICIENTS
if [ $# -gt 0 ]; then
	COEFFICIENT_LIST=$@;
	parse_coeff_list
fi

# CHECK BINARIES
if [ -z $PAG_GEN_BINARIES ]; then
	PAG_GEN_BINARIES=$PWD
fi

if [ -x $PAG_GEN_BINARIES/rpag ]; then 
	RPAG_BIN="$PAG_GEN_BINARIES/rpag"; 
else
	failure "BIN_CHECK" "Could not find rpag in binaries path ('$PAG_GEN_BINARIES')"
	exit 0
fi

if [ -x $PAG_GEN_BINARIES/pag_print ]; then 
	PRINT_BIN="$PAG_GEN_BINARIES/pag_print"; 
fi

if [ -x $PAG_GEN_BINARIES/pag_split ]; then 
	SPLIT_BIN="$PAG_GEN_BINARIES/pag_split"; 
elif [ $CFG_COUNT -gt 1 ]; then 
	failure "BIN_CHECK" "Could not find pag_split in binaries path ('$PAG_GEN_BINARIES')"
	exit 0
fi

if [ -x $PAG_GEN_BINARIES/pag_fusion ]; then 
	FUSION_BIN="$PAG_GEN_BINARIES/pag_fusion"; 
elif [ $CFG_COUNT -gt 1 ]; then 
	failure "BIN_CHECK" "Could not find pag_fusion in binaries path ('$PAG_GEN_BINARIES')"
fi

if [ -x $PAG_GEN_BINARIES/flopoco ]; then 
	FLOPOCO_BIN="$PAG_GEN_BINARIES/flopoco"; 
elif $GENERATE_VHDL ; then 
	failure "BIN_CHECK" "Could not find flopoco in binaries path ('$PAG_GEN_BINARIES')"
fi

if [ -x $PAG_GEN_BINARIES/simulate.sh ]; then 
	SIMULATE_BIN="$PAG_GEN_BINARIES/simulate.sh"; 
elif $GENERATE_SIM ; then 
	failure "BIN_CHECK" "Could not find simulate-script in binaries path ('$PAG_GEN_BINARIES')"
fi

if [ -x $PAG_GEN_BINARIES/$SYNTH_SCRIPT_NAME ]; then 
	SYNTH_BIN="$PAG_GEN_BINARIES/$SYNTH_SCRIPT_NAME"; 
	if [ -f $PAG_GEN_BINARIES/template.txt ]; then
		echo "" > "$PAG_GEN_BINARIES/template.txt"
	fi
	SYNTH_TEMPLATE="$PAG_GEN_BINARIES/template.txt";
elif $SYNTHESIZE ; then 
	failure "BIN_CHECK" "Could not find synth-script in binaries path ('$PAG_GEN_BINARIES')"
fi



# PREPARATION STEPS
if [ ${#COPY_SUFFIX} -gt 0 ]; then
	if [ ! -d $OUTPUT_DIR/${INSTANCE_NAME} ]; then
		failure "COPY_SUFFIX" "Can't copy non existing instance"
	fi
	
	COPY_SUFFIX="_${COPY_SUFFIX}";
	
	if $CLEAN; then	
		rm -Rf $OUTPUT_DIR/${INSTANCE_NAME}${COPY_SUFFIX}/*
	fi
	
	cp -ar $OUTPUT_DIR/${INSTANCE_NAME}/ $OUTPUT_DIR/${INSTANCE_NAME}${COPY_SUFFIX}
fi

if [ ! -d $OUTPUT_DIR/${INSTANCE_NAME} ]; then
	if [ "$BEGIN_WITH" != "rpag" ]; then
		failure "BEGIN_WITH" "Can't begin with $BEGIN_WITH no folder exists"
	fi

	mkdir -p "$OUTPUT_DIR/$INSTANCE_NAME"
	check_failure "MKDIR" "Failed to create output directory"
	if $CLEAN; then	
		rm -Rf $OUTPUT_DIR/$INSTANCE_NAME/*
	fi
fi

if [ ${#COEFFICIENT_LIST} -eq 0 ]; then
	if [ -f ${OUTPUT_DIR}/${INSTANCE_NAME}${COPY_SUFFIX}/${INSTANCE_NAME}_coefficients.txt ]; then
		COEFFICIENT_LIST=`cat ${OUTPUT_DIR}/${INSTANCE_NAME}${COPY_SUFFIX}/${INSTANCE_NAME}_coefficients.txt`
		parse_coeff_list
	else
		failure "CHECK" "No coefficients given or found";
	fi
else
	echo $COEFFICIENT_LIST > ${OUTPUT_DIR}/${INSTANCE_NAME}${COPY_SUFFIX}/${INSTANCE_NAME}_coefficients.txt
fi

if $TERNARY_MODE ; then
	RPAG_ARGS="--ternary_adders $RPAG_ARGS"
fi
if [ $INPUT_COUNT -gt 1 ]; then
	RPAG_ARGS="--cmm $RPAG_ARGS"
fi

# CHECK AUTOMODE
if $AUTOMATIC_MODE;then
	BEGIN_WITH="rpag"
	if [ -f ${OUTPUT_DIR}/${INSTANCE_NAME}${COPY_SUFFIX}/${INSTANCE_NAME}_rpag_output_graph.txt ]; then
		if [ $CFG_COUNT -gt 1 ]; then 
			BEGIN_WITH="split"
			if [ -f ${OUTPUT_DIR}/${INSTANCE_NAME}${COPY_SUFFIX}/${INSTANCE_NAME}_pag_fusion_input.txt ]; then
				BEGIN_WITH="fusion"
				if [ -f ${OUTPUT_DIR}/${INSTANCE_NAME}${COPY_SUFFIX}/${INSTANCE_NAME}_fusion_output_graph.txt ]; then
					if $GENERATE_VHDL; then 
						BEGIN_WITH="flopoco"; 
						if [ -f ${OUTPUT_DIR}/${INSTANCE_NAME}${COPY_SUFFIX}/${INSTANCE_NAME}_realisation.vhdl ]; then
							if $SYNTHESIZE; then 
								BEGIN_WITH="synth"; 
								if [ -f ${OUTPUT_DIR}/${INSTANCE_NAME}${COPY_SUFFIX}/synth/PipelinedFlexibleMultiplication_${VHDL_INPUT_WORDSIZE}_summary.xml ]; then
									BEGIN_WITH="finish";
								fi
							else  
								BEGIN_WITH="finish"; 
							fi
						fi						
					else  
						BEGIN_WITH="finish"; 
					fi
				fi
			fi
		elif $GENERATE_VHDL; then
			BEGIN_WITH="flopoco"
			if [ -f ${OUTPUT_DIR}/${INSTANCE_NAME}${COPY_SUFFIX}/${INSTANCE_NAME}_realisation.vhdl ]; then
				if $SYNTHESIZE; then 
					BEGIN_WITH="synth"; 
					if [ -f ${OUTPUT_DIR}/${INSTANCE_NAME}${COPY_SUFFIX}/synth/PipelinedFlexibleMultiplication_${VHDL_INPUT_WORDSIZE}_summary.xml ]; then
						BEGIN_WITH="finish";
					fi
				else  
					BEGIN_WITH="finish"; 
				fi
			fi
		else
			BEGIN_WITH="print"
			if [ -f ${OUTPUT_DIR}/${INSTANCE_NAME}${COPY_SUFFIX}/${INSTANCE_NAME}_output_graph.dot ]; then
				BEGIN_WITH="finish";
			fi
		fi
	fi
fi

if [ "$BEGIN_WITH" == "finish" ]; then
	echo "[AUTO] All jobs already completed"
	exit 0
fi

# START RUN
cd $OUTPUT_DIR/$INSTANCE_NAME
OPERATION=$BEGIN_WITH

while true;
do

case $OPERATION in
rpag)
	RPAG_INPUT_LIST=${COEFFICIENT_LIST}
	if $RUN_RPAG_TOXED; then
		RPAG_ARGS="${RPAG_ARGS} --cmm"
		RPAG_INPUT_LIST=`echo "1,${COEFFICIENT_LIST}" | sed 's/;/;1,/g' | sed 's/ / 1,/g'`
	fi
	echo "[CALL] rpag ${RPAG_ARGS} ${RPAG_INPUT_LIST}"
	$RPAG_BIN ${RPAG_ARGS} ${RPAG_INPUT_LIST} >"${INSTANCE_NAME}_rpag.out" 2>&1
	
	RPAG_OUTPUT_GRAPH=`cat ${INSTANCE_NAME}_rpag.out | grep 'pipelined_adder_graph' | cut -d= -f2`
	if [ ${#RPAG_OUTPUT_GRAPH} -gt 0 ]; then
		if $RUN_RPAG_TOXED; then
			echo "[CALL] pag_split \"${RPAG_OUTPUT_GRAPH}\" --graph_detox --instance=\"${INSTANCE_NAME}\""
			$SPLIT_BIN "${RPAG_OUTPUT_GRAPH}" --graph_detox --instance="${INSTANCE_NAME}" 2>&1
			RPAG_OUTPUT_GRAPH=`cat ${INSTANCE_NAME}_detox.txt`
		fi
		echo $RPAG_OUTPUT_GRAPH > "${INSTANCE_NAME}_rpag_output_graph.txt"
	else
		failure "RPAG" "No rpag output graph found"
	fi
	
	if [ $CFG_COUNT -gt 1 ]; then 
		OPERATION="split"
	elif $GENERATE_VHDL; then
		OPERATION="flopoco"
	else
		OPERATION="print"
	fi
;;
split)
	if [ -f "${INSTANCE_NAME}_rpag_output_graph.txt" ]; then
		SPLIT_INPUT=`cat ${INSTANCE_NAME}_rpag_output_graph.txt`
	else
		failure "PAG_SPLIT" "No input file found"
		exit -1
	fi
	
	echo "[CALL] pag_split \"${SPLIT_INPUT}\" \"${COEFFICIENT_LIST}\" --pag_fusion_input --instance=\"${INSTANCE_NAME}\""
	$SPLIT_BIN "${SPLIT_INPUT}" "${COEFFICIENT_LIST}" --pag_fusion_input --instance="${INSTANCE_NAME}" >"${INSTANCE_NAME}_split.out" 2>&1
	
	if [ "$?" != "0" ]; then
		failure "PAG_SPLIT" "Exited with errors"
		exit -1
	fi
	
	if [ ! -f "${INSTANCE_NAME}_pag_fusion_input.txt" ]; then
		failure "PAG_SPLIT" "No pag_split output file found"
		exit -1
	fi	
	
	OPERATION="fusion"
;;
fusion)
	if [ ! -f "${INSTANCE_NAME}_pag_fusion_input.txt" ]; then
		failure "PAG_FUSION" "No input file found"
		exit -1
	fi
	
	echo "[CALL] pag_fusion --if \"${INSTANCE_NAME}_pag_fusion_input.txt\" --ofpre ${INSTANCE_NAME} ${FUSION_ARGS}"
	$FUSION_BIN --if "${INSTANCE_NAME}_pag_fusion_input.txt" --ofpre "${INSTANCE_NAME}_fusion" ${FUSION_ARGS} > "${INSTANCE_NAME}_fusion.out" 2>&1
	
	if [ "$?" != "0" ]; then
		failure "PAG_FUSION" "Exited with errors"
		exit -1
	fi
	
	if [ ! -f "${INSTANCE_NAME}_fusion_output_graph.txt" ]; then
		failure "PAG_FUSION" "No pag_fusion output file found"
		exit -1
	fi	
	
	if $GENERATE_VHDL; then OPERATION="flopoco"; else  OPERATION="finish"; fi
;;
flopoco)
	if [ -f "${INSTANCE_NAME}_fusion_output_graph.txt" ]; then
		FLOPOCO_INPUT=`cat ${INSTANCE_NAME}_fusion_output_graph.txt`
	else
		if [ -f "${INSTANCE_NAME}_rpag_output_graph.txt" ]; then
			FLOPOCO_INPUT=`cat ${INSTANCE_NAME}_rpag_output_graph.txt`
		else
			failure "FLOPOCO" "No input file found"
			exit -1
		fi
	fi
	
	if $GENERATE_SIM; then FLOPOCO_SIM_ARGS="-name=${INSTANCE_NAME}_test TestBench ${SIM_SIZE}"; fi
	
	echo "[CALL] flopoco ${FLOPOCO_ARGS} ConstMultPAGx ${VHDL_INPUT_WORDSIZE} ${FLOPOCO_SYNC_OPT} \"${FLOPOCO_INPUT}\" ${FLOPOCO_SIM_ARGS}"
	$FLOPOCO_BIN ${FLOPOCO_ARGS} ConstMultPAGx ${VHDL_INPUT_WORDSIZE} ${FLOPOCO_SYNC_OPT} "${FLOPOCO_INPUT}" ${FLOPOCO_SIM_ARGS}  >"${INSTANCE_NAME}_flopoco.out" 2>&1
	
	if [ "$?" != "0" ]; then
		failure "FLOPOCO" "Exited with errors"
		exit -1
	fi
	
	if [ -f "gmon.out" ]; then rm gmon.out; fi
	
	if [ -f "flopoco.vhdl" ]; then
		mv flopoco.vhdl "${INSTANCE_NAME}_realisation.vhdl"
	else
		failure "FLOPOCO" "No flopoco output file found"
		exit -1
	fi
	if $GENERATE_SIM; then OPERATION="simulate";
	else 
		if $SYNTHESIZE; then OPERATION="synth"; 
		else  OPERATION="finish";
		fi
	fi
;;
simulate)
	if [ ! -f "${INSTANCE_NAME}_realisation.vhdl" ]; then
		failure "SIMULATE" "No input file found"
	fi 
	
	echo "[CALL] simulate.sh ${INSTANCE_NAME}_test ${INSTANCE_NAME}_realisation.vhdl"
	$SIMULATE_BIN ${INSTANCE_NAME}_test ${INSTANCE_NAME}_realisation.vhdl > ${INSTANCE_NAME}_simulation.out
	
	if [ ! $? -eq 0 ]; then
		failure "SIMULATE" "Simulation not successfull"
	fi
	if $SYNTHESIZE; then OPERATION="synth"; 
	else  OPERATION="finish"; 
	fi	
;;
synth)
	if [ ! -f "${INSTANCE_NAME}_realisation.vhdl" ]; then
		failure "SYNTH" "No input file found"
		exit -1
	fi
	
	SYNTH_ENTITY=`cat ${INSTANCE_NAME}_realisation.vhdl | grep "entity" | grep "PipelinedFlexibleMultiplication" | head -n 1 | cut -d' ' -f2`
	
	if [ ! -d synth ];then mkdir synth; fi
	cp ${INSTANCE_NAME}_realisation.vhdl synth/${INSTANCE_NAME}_realisation.vhd
	cd synth
	
	echo "[CALL] $SYNTH_SCRIPT_NAME speed_no_ret_no_dup ${SYNTH_ENTITY} "${INSTANCE_NAME}_realisation.vhd "$PAG_GEN_BINARIES/template.txt"
	$SYNTH_BIN speed_no_ret_no_dup ${SYNTH_ENTITY} "${INSTANCE_NAME}_realisation.vhd" "$PAG_GEN_BINARIES/template.txt" >../"${INSTANCE_NAME}_synth.out" 2>&1

	if [ "$?" != "0" ]; then
		failure "SYNTH" "Exited with errors"
		exit -1
	fi
	
	if [ ! -f ${SYNTH_ENTITY}_summary.xml ]; then
		failure "SYNTH" "Synth not completed"
		exit -1
	fi
	
	cd ..
	OPERATION="finish"
;;
print)
	if [ -f "${INSTANCE_NAME}_rpag_output_graph.txt" ]; then
		PRINT_INPUT=`cat ${INSTANCE_NAME}_rpag_output_graph.txt`
	else
		failure "PAG_PRINT" "No input file found"
		exit -1
	fi
	
	echo "[CALL] pag_print ${PRINT_INPUT}"
	$PRINT_BIN ${PRINT_INPUT} >"${INSTANCE_NAME}_print.out" 2>&1
	if [ $? -eq 0 -a -f "pag_adder_graph.dot" ]; then
		mv "pag_adder_graph.dot" "${INSTANCE_NAME}_output_graph.dot"
	else
		failure "PAG_PRINT" "Error printing graph"
		exit 0
	fi
	
	OPERATION="finish"
;;
finish)
	dot -Tpdf -O *.dot >/dev/null 2>&1
	break
;;
esac
done

exit 0
