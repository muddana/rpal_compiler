p1:	
	g++ parser.c++ -o p1
cl: 
	rm -f p1;
run:
	./p1
test:
	perl ./difftest.pl -1 "rpal FILE" -2 "./p1 -r FILE" -t ../../rpal/interpreters/rpal/tests/