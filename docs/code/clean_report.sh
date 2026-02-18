#!/bin/bash

if [ $# -ne 1 ]; then
	exit 1
fi

input_pdf="$1"
tmp_file=$(mktemp --suffix=.pdf)

gs -sDEVICE=pdfwrite \
	-dCompatibilityLevel=1.4 \
	-dNOPAUSE \
	-dOptimize=true \
	-dQUIET \
	-dBATCH \
	-dRemoveUnusedFonts=true \
	-dRemoveUnusedImages=true \
	-dOptimizeResources=true \
	-dDetectDuplicateImages \
	-dCompressFonts=true \
	-dEmbedAllFonts=true \
	-dSubsetFonts=true \
	-dPreserveAnnots=true \
	-dPreserveMarkedContent=true \
	-dPreserveOverprintSettings=true \
	-dPreserveHalftoneInfo=true \
	-dPreserveOPIComments=true \
	-dPreserveDeviceN=true \
	-dMaxInlineImageSize=0 \
	-sOutputFile="$tmp_file" \
	"$input_pdf"

mv "$tmp_file" "$input_pdf"
