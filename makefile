all: electronics/electrical.png software/fsm.png

%.svg: %.dot
	dot -Tsvg $< > $@

%.png: %.svg
	convert -density 200 -resize 600 $< $@

clean:
	rm -f electrical/electrical.png software/fsm.png

