CC=oshcc
CFLAGS=-Wall -Wextra -Werror -Wno-unused -Wno-unused-parameter -Wno-format-overflow -g3 -O3 -msse4.2

all: hello_world array broadcast

hello_world: hello_world.c
	        $(CC) -o $@ $<

array: array.c
	        $(CC) -o $@ $<
		
broadcast: broadcast.c
	        $(CC) -o $@ $<

team: team.c
		 $(CC) -o $@ $<

monte_carlo_pi: monte_carlo_pi.c
		 $(CC) -o $@ $<
