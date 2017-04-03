m1: m1.o
	g++ -L/usr/local/db6/lib -L/home/st/stromb/cpsc5300/sql-parser -o $@ $< -ldb_cxx -lsqlparser

m1.o : milestone1.cpp
	g++ -g -I/usr/local/db6/include -DHAVE_CXX_STDHEADERS -D_GNU_SOURCE -D_REENTRANT -I/home/st/stromb/cpsc5300/sql-parser/src -O3 -std=c++11 -c -o $@ $<

clean: 
	rm m1.o
	rm m1 