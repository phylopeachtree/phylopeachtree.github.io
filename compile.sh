#!/bin/bash



# Compile java into jar (requires java version 8)
echo "Compiling java..."
cd java/src/
#cp ../lib/json-20201115.jar .
javac -d . -cp ../lib/json-20201115.jar:../lib/annotations-2.0.1.jar:../lib/guava-15.0.jar:../lib/javassist-3.19.0-GA.jar:../lib/reflections-0.9.10.jar peachtree/aln/AlignmentAPI.java peachtree/aln/Alignment.java peachtree/aln/Taxon.java peachtree/aln/colourings/JalviewNucleotideColouring.java peachtree/aln/colourings/ClustalAminoColouring.java  peachtree/aln/colourings/Colouring.java peachtree/aln/Filtering.java peachtree/aln/Sequence.java peachtree/options/OptionsAPI.java peachtree/options/Scaling.java peachtree/options/Option.java peachtree/options/NumericalOption.java peachtree/options/DiscreteOption.java peachtree/MainApplication.java


echo "Building executable jar..."
jar vcmf ../MANIFEST.MF peachtree.jar peachtree/aln/AlignmentAPI.class peachtree/aln/Alignment.class peachtree/aln/Taxon.class peachtree/aln/colourings/JalviewNucleotideColouring.class peachtree/aln/colourings/ClustalAminoColouring.class peachtree/aln/colourings/Colouring.class peachtree/aln/Filtering.class peachtree/aln/Sequence.class peachtree/options/OptionsAPI.class peachtree/options/Scaling.class peachtree/options/Option.class peachtree/options/NumericalOption.class peachtree/options/DiscreteOption.class peachtree/MainApplication.class 
mv peachtree.jar ../../.
#rm json-20201115.jar
cd ../../


# Compile jar into js using cheerPJ (assumes that cheerpj is installed at ~/cheerpj_2.1/)


# Compile all dependencies. Assumes (wrongly but safely) that each library is a dependency of each other library
#deps=("json-20201115.jar" "annotations-2.0.1.jar" "guava-15.0.jar" "javassist-3.19.0-GA.jar" "reflections-0.9.10.jar")
deps=("json-20201115.jar")
deps_str="${deps[@]}"
depsSepByColon="${deps_str// /\:}"
echo $depsSepByColon
for d in ${deps[@]}; do

	if [ ! -f "$d.js" ];  then
		echo "Compiling $d library..."
		cp java/lib/$d .
		#~/cheerpj_2.1/cheerpjfy.py --deps $depsSepByColon $d
		../cheerpj/cheerpj_2.1/cheerpjfy.py --deps $depsSepByColon $d
	fi

done



#:../lib/annotations-2.0.1.jar:../lib/guava-15.0.jar:../lib/javassist-3.19.0-GA.jar:../lib/reflections-0.9.10.jar

echo "Compiling jar into js..."
#~/cheerpj_2.1/cheerpjfy.py --deps $depsSepByColon peachtree.jar
../cheerpj/cheerpj_2.1/cheerpjfy.py --deps $depsSepByColon peachtree.jar





