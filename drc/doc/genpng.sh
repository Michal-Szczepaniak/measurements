#!/bin/sh

# Create the PNG version of all eps files
for file in figures/*.eps
do
	pngfile=$(basename "$file" .eps)
	echo "$file" '->' "${pngfile}.png"	
	gs -q -dEPSFitPage -g1024x768 -dNOPAUSE -dBATCH -dTextAlphaBits=4 -dGraphicsAlphaBits=4 -sDEVICE=png16m -sOutputFile="${pngfile}.png" "$file"
done
