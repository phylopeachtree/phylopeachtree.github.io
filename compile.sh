#!/bin/bash



# Compile java into jar (requires java version 8)
echo "Compiling java..."
cd java/src/
#cp ../lib/json-20201115.jar .
javac -d . -cp ../lib/json-20201115.jar  peachtree/aln/AlignmentAPI.java peachtree/aln/Alignment.java peachtree/aln/Taxon.java peachtree/aln/NucleotideColouring.java  peachtree/aln/Colouring.java peachtree/aln/Filtering.java peachtree/aln/Sequence.java peachtree/options/OptionsAPI.java peachtree/options/Scaling.java peachtree/options/Option.java peachtree/options/NumericalOption.java peachtree/MainApplication.java


echo "Building executable jar..."
jar vcmf ../MANIFEST.MF peachtree.jar peachtree/aln/AlignmentAPI.class peachtree/aln/Alignment.class peachtree/aln/Taxon.class peachtree/aln/NucleotideColouring.class peachtree/aln/Colouring.class peachtree/aln/Filtering.class peachtree/aln/Sequence.class peachtree/options/OptionsAPI.class peachtree/options/Scaling.class peachtree/options/Option.class peachtree/options/NumericalOption.class peachtree/MainApplication.class 
mv peachtree.jar ../../.
#rm json-20201115.jar
cd ../../


# Compile jar into js using cheerPJ (assumes that cheerpj is installed at ~/cheerpj_2.1/)







if [ ! -f json-20201115.jar.js ];  then
	echo "Compiling json library..."
	cp java/lib/json-20201115.jar .
    	~/cheerpj_2.1/cheerpjfy.py json-20201115.jar
fi


echo "Compiling jar into js..."
~/cheerpj_2.1/cheerpjfy.py --deps json-20201115.jar peachtree.jar






