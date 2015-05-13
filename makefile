all: electrical.png software.png

%.svg: %.dot
	dot -Tsvg $< > $@

%.png: %.svg
	convert -density 200 -resize 800 $< $@

