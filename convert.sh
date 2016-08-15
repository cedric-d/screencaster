#!/bin/sh

OUTDIR=$1

if [ ! -d "$OUTDIR" ]
then
	echo "First argument is not a directory" >&2
	exit 1
fi

shift

refimg=

for img in "$@"
do
	outfile="$OUTDIR/${img##*/}"
	
	echo "Converting $img to $outfile..."
	
	# extract metadata
	timestamp=
	xpos=0
	ypos=0
	eval "$(identify -verbose "$img" \
	  | sed -e '/^  Properties:$/,/^  [^ ]/!d' \
	        -e 's/^    //' -e 's/: /="/' -e 's/$/"/' \
		-n -r -e '/^(timestamp|xpos|ypos)=/p')"
	
	# copy the first image as-is
	if [ -z "$refimg" ]
	then
		cp $img $outfile
	else
		composite -geometry +${xpos}+${ypos} "$img" "$refimg" "$outfile"
	fi
	
	refimg=$outfile
done
