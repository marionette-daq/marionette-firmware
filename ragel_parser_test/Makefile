
all: parser_test fetch_parser.png fetch_parser.svg string_parser.png string_parser.svg gpio_parser.png gpio_parser.svg

parser_test: parser_test.c
	gcc -g -o parser_test -g parser_test.c

parser_test.c: parser_test.rl
	ragel -C parser_test.rl -o parser_test.c

fetch_parser.dot: parser_test.rl
	ragel -V -S fetch_parser parser_test.rl -o fetch_parser.dot

fetch_parser.png: fetch_parser.dot
	dot -Tpng fetch_parser.dot -o fetch_parser.png

fetch_parser.svg: fetch_parser.dot
	dot -Tsvg fetch_parser.dot -o fetch_parser.svg

string_parser.dot: parser_test.rl
	ragel -V -S string_parser parser_test.rl -o string_parser.dot

string_parser.png: string_parser.dot
	dot -Tpng string_parser.dot -o string_parser.png

string_parser.svg: string_parser.dot
	dot -Tsvg string_parser.dot -o string_parser.svg

gpio_parser.dot: parser_test.rl
	ragel -V -S gpio_parser parser_test.rl -o gpio_parser.dot

gpio_parser.png: gpio_parser.dot
	dot -Tpng gpio_parser.dot -o gpio_parser.png

gpio_parser.svg: gpio_parser.dot
	dot -Tsvg gpio_parser.dot -o gpio_parser.svg

clean:
	rm fetch_parser.dot fetch_parser.png fetch_parser.svg
	rm string_parser.dot string_parser.png string_parser.svg
	rm gpio_parser.dot gpio_parser.png gpio_parser.svg
	rm parser_test.c parser_test
