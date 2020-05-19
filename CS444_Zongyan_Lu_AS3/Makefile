#
# R Jesse Chaney
#

# CC = gcc
# DEBUG = -g
# DEFINES =
# DEFINES += -DCHECK
#
# CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
#         -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
#         -Wdeclaration-after-statement
# PROG = mystat


all: mystat


mystat: mystat.c
	gcc -o mystat mystat.c -g -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
	-Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
	-Wdeclaration-after-statement


# stat: stat.c
# 	$(CC) $(CFLAGS) -o mystat
# 	chmod a+rx,g-w $@

# clean up the compiled files and editor chaff
clean:
	rm -rf mystat
