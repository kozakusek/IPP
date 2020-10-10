#!/bin/bash

VAL="valgrind -q --error-exitcode=15 --leak-check=full \
	 --show-leak-kinds=all --errors-for-leak-kinds=all"

[ $# -ne 2 ] && { echo Wrong nubmer of arguments; exit 1; }

for f in $2/*.in
do
	echo -e "Processing: \e[1m$(basename "$f")\e[0m"

	${VAL} "$(dirname "$1")"/"$(basename "$1")" <$f >tmp.out 2>tmp.err
	echo "For file $(basename "$f") program returned code $?."

	if diff ${f%in}out tmp.out >/dev/null
		then echo -e "\e[32mOutput test passed\e[0m"
	else
		echo -e "\e[91m\e[5mOutput test failed\e[0m"
	fi;

	if diff ${f%in}err tmp.err >/dev/null
		then echo -e "\e[32mError test passed\e[0m"
	else
		echo -e "\e[91m\e[5mError test failed\e[25m\e[0m"
	fi;

	rm -f tmp.out tmp.err
	
done

read -p "Press enter"