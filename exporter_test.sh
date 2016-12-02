#!/bin/bash

# Stop on failure
set -eu

EXPORTERS=(make_armc5 make_gcc_arm make_iar uvision5 iar)
PASSED=()

fail() {
	echo "Failed on $1"
	return 1
}

pass() {
	PASSED+=($1)
}

for tool in ${EXPORTERS[*]};do
	echo "Testing on $tool"
	mbed export -m K64F -i $tool
	# Makefile builds
	if [[ $tool = make_* ]]; then
		make || fail $tool && pass $tool
	elif [[ $tool = uvision5 ]]; then
		# Keil return ERRORLEVEL 0, when build OK without warnings
		# ERRORLEVEL 1, when there was warnings. 2 or over, is not OK.
		"c:/Keil_v5/UV4/UV4.exe" -b mbed-os-example-mesh-minimal.uvprojx || \
		test $? -lt 2 || fail $tool && pass $tool
	elif [[ $tool = iar ]]; then
		"C:/Program Files (x86)/IAR Systems/Embedded Workbench 7.5/common/bin/IarBuild.exe" mbed-os-example-mesh-minimal.ewp mbed-os-example-mesh-minimal \
			|| fail $tool && pass $tool
	fi
	mv BUILD BUILD_$tool
done

printf "\n|%15s |%6s |\n" "Tool" "Result"
printf "|----------------|-------|\n"
for tool in ${PASSED[*]};do
	printf "|%15s |%6s |\n" $tool PASS
done
