
# Makefile for graphiz diagrams

DOT=dot 
DOTDIR=.

dotsources:=$(wildcard $(DOTDIR)/*.dot)
pngfiles=$(dotsources:%.dot=%.png)

.SUFFIXES: .dot .png

%.png: %.dot
	$(DOT) -Tpng $< > $@

all: $(pngfiles)

clean:
	$(RM) $(pngfiles)

