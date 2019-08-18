#!/bin/sh


print_help_text () {

	echo "Usage: inflate.sh duration"
	exit 1

}

start_attack () {

	# Spin up cpu eaters
	for i in {1..$1}
	do
		yes > /dev/null &
	done

}


end_attack () {

	killall yes

}


main () {

	if [ "$1" == "-h" ]; then
		print_help_text
	fi

	# Read in duration
	if ! [[ "$1" =~ ^[0-9]+$ ]]; then
		print_help_text
	else
		echo "Running for $1 seconds"
	fi


	start_attack 10

	sleep $1

	end_attack

}


main $1
