all: as1 run check

2: 2.c
	gcc -o 2.out 2.c

3: 3.c
	gcc -o 3.out 3.c

4: 4.c
	gcc -o 4.out 4.c

5: 5.c
	gcc -o 5.out 5.c

as1: 2 3 4 5

run:
	2.out > 2_output.txt
	3.out
	4.out > 4_output.txt
	5.out > 5_output.txt

check: 
	cat 2_output.txt
	cat JUNK.txt
	cat 4_output.txt
	cat 5_output.txt

clean:
	rm -rf *.out *.txt
