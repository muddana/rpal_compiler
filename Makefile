p1:	
	g++ main.c++ -o ./bin/p1
clean: 
	rm -f ./bin/p1;
test:
	perl ./difftest.pl -1 "rpal FILE" -2 "./bin/p1 -r FILE" -t ./tests/