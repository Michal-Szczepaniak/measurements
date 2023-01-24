#!/bin/sh

# Create the pdf version of all eps files
for file in figures/*.eps
do
	pdffile=$(basename "$file" .eps)
	echo "$file" '->' "${pdffile}.pdf"
	BBox=$(gs -q -dBATCH -dNOPAUSE -sDEVICE=bbox "$file" 2>&1 | grep '%%HiResBoundingBox:' | cut -d ':' -f 2)
	gs -q -dBATCH -dNOPAUSE -o "${pdffile}.pdf" -sDEVICE=pdfwrite -dAutoFilterColorImages=false -dColorImageFilter=/FlateEncode -dPDFSETTINGS=/prepress -dColorImageResolution=600 -dGrayImageResolution=600 -c "[/CropBox [${BBox}]" -c "/PAGE pdfmark" -f "$file" 
done
