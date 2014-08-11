plot: Test.csv
	gnuplot -persist Plot.gplt
	
Test.csv:
	g++ -o bench main.cpp -ldl
	python main.py
		
clean:
	rm -f *.csv bench
