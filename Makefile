FILES := main.c mtx.c

all: main

main: ${FILES}
	gcc -Wall -Werror -omain ${FILES}