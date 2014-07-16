#!/bin/sh

# Assumption: instrument parameter value 0 of the Silvet plugin is
# always "multiple or unknown instruments" and 1 is always "piano".

piano_path="/home/cannam/Music/piano_small_dataset"
yc="/home/cannam/code/may/bin/yc"

if [ ! -d "$piano_path" ]; then
    echo "Piano dataset directory $piano_path not found, giving up"
    exit 1
fi

if ! "$yc" -v ; then
    echo "Failed to run Yeti compiler yc at $yc_path, giving up";
fi

if ! sonic-annotator -v ; then
    echo "Failed to run sonic-annotator (not in PATH?), giving up"
    exit 1
fi

rdffile="../../silvet.n3"
if [ ! -f "$rdffile" ] ; then
    echo "Failed to find plugin RDF file at $rdffile, giving up"
    exit 1
fi

case "$piano_path" in
*\ *) echo "Piano dataset path $piano_path has a space in it, this script won't handle that"; exit 1;;
esac

( cd ../.. ; make -f Makefile.linux ) || exit 1

VAMP_PATH=../..
export VAMP_PATH

outfile="/tmp/$$"

tmpwav="/tmp/$$.wav"

transfile="/tmp/$$transform.ttl"

trap 'rm -f "$outfile" "$tmpwav" "$instfile" "$transfile" "$outfile.lab"' 0

infiles=$(find "$piano_path" -name \*.wav)

echo
echo "Input files are:"
echo $infiles | fmt -1

time for infile in $infiles; do

    echo
    echo "Evaluating for file $infile..."

    intended_instrument=1 ## assumption: 1 == piano

    # We run this twice, once using the default instrument
    # (i.e. "multiple or unknown") and once using the intended
    # instrument preset (piano).

    filename=$(basename "$infile" .wav)

    for instrument in $intended_instrument 0; do

	echo
	echo "For file $filename, instrument $instrument..."

	# Don't normalise -- part of the point here is to make it work
	# for various different levels
	cp "$infile" "$tmpwav"

	# generate the transform by interpolating the instrument parameter
	cat transform.ttl | sed "s/INSTRUMENT_PARAMETER/$instrument/" > "$transfile"

	sonic-annotator \
	    --writer csv \
	    --csv-one-file "$outfile" \
	    --csv-force \
	    --transform "$transfile" \
	    "$tmpwav"

	cat "$outfile" | \
	    sed 's/^[^,]*,//' | \
	    while IFS=, read start duration frequency level label; do
	    end=`echo "$start $duration + p" | dc`
	    echo -e "$start\t$end\t$frequency"
	done > "$outfile.lab"

	for ms in 50; do
	    mark=""
	    if [ "$instrument" = "0" ]; then
		mark="  <-- generic for $filename"; 
	    else
		mark="  <-- piano preset for $filename";
	    fi;
	    echo
	    echo "Validating against ground truth at $ms ms:"
	    "$yc" ./evaluate_lab.yeti "$ms" "../piano-groundtruth/$filename.lab" "$outfile.lab" | sed 's,$,'"$mark"','
	done;

	echo
    done
done
