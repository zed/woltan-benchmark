LIBBLAS_SO := libblas.so

plot: Test.csv
	gnuplot -persist Plot.gplt

Test.csv:
	g++ -DLIBBLAS_SO=$(LIBBLAS_SO) -o bench main.cpp -ldl
	python main.py $(LIBBLAS_SO)

clean:
	rm -f *.csv bench
