all: electrical.png software.png

%.svg: %.dot
	dot -Tsvg $< > $@

%.png: %.svg
	convert -density 200 -resize 600 $< $@

clean:
	rm -f electrical.png software.png

