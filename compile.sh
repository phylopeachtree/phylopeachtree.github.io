#!/bin/bash



# Compile java into jar (requires java version 8)
echo "Compiling java..."
cd java/src/
#cp ../lib/json-20201115.jar .
javac -d . -cp ../lib/json-20201115.jar:/../lib/antlr-runtime-4.7.jar peachtree/aln/AlignmentAPI.java peachtree/aln/Alignment.java peachtree/aln/Taxon.java peachtree/epi/EpiAPI.java peachtree/epi/Epidemiology.java peachtree/epi/Case.java peachtree/aln/colourings/Jalview.java peachtree/aln/colourings/Aliview.java peachtree/aln/colourings/SiteColourFilter.java peachtree/aln/colourings/Drums.java peachtree/aln/colourings/ClustalAmino.java  peachtree/aln/colourings/Colouring.java peachtree/aln/Filtering.java peachtree/aln/Sequence.java peachtree/options/OptionsAPI.java peachtree/options/Scaling.java peachtree/phy/PhylogenyAPI.java peachtree/phy/Tree.java peachtree/phy/Node.java peachtree/phy/ClusterTree.java  peachtree/options/Option.java peachtree/options/NumericalOption.java peachtree/options/Option.java peachtree/options/BooleanOption.java  peachtree/options/DiscreteOption.java peachtree/MainApplication.java peachtree/phy/util/Tuple.java peachtree/phy/util/TupleComparator.java peachtree/phy/util/NodeX.java peachtree/phy/util/LinkType.java


echo "Building executable jar..."
jar vcmf ../MANIFEST.MF peachtree.jar peachtree/aln/AlignmentAPI.class peachtree/aln/Alignment.class peachtree/aln/Taxon.class peachtree/epi/EpiAPI.class peachtree/epi/Epidemiology.class peachtree/epi/Case.class peachtree/aln/colourings/Jalview.class peachtree/aln/colourings/SiteColourFilter.class peachtree/aln/colourings/Aliview.class peachtree/aln/colourings/Drums.class peachtree/aln/colourings/ClustalAmino.class peachtree/aln/colourings/Colouring.class peachtree/aln/Filtering.class peachtree/aln/Sequence.class peachtree/options/OptionsAPI.class peachtree/options/Scaling.class peachtree/options/Option.class peachtree/options/NumericalOption.class peachtree/options/BooleanOption.class peachtree/options/DiscreteOption.class peachtree/MainApplication.class peachtree/phy/PhylogenyAPI.class peachtree/phy/Tree.class peachtree/phy/Node.class peachtree/phy/ClusterTree.class peachtree/phy/util/Tuple.class peachtree/phy/util/TupleComparator.class peachtree/phy/util/NodeX.class peachtree/phy/util/LinkType.class
mv peachtree.jar ../../.
#rm json-20201115.jar
cd ../../


# Compile jar into js using cheerPJ (assumes that cheerpj is installed at ~/cheerpj_2.1/)


# Compile all dependencies. Assumes (wrongly but safely) that each library is a dependency of each other library
#deps=("json-20201115.jar" "annotations-2.0.1.jar" "guava-15.0.jar" "javassist-3.19.0-GA.jar" "reflections-0.9.10.jar")
deps=("json-20201115.jar" "antlr-runtime-4.7.jar")
deps_str="${deps[@]}"
depsSepByColon="${deps_str// /\:}"
echo $depsSepByColon
for d in ${deps[@]}; do

	if [ ! -f "$d.js" ];  then
		echo "Compiling $d library..."
		cp java/lib/$d .
		~/cheerpj_2.1/cheerpjfy.py --deps $depsSepByColon $d
		#../cheerpj/cheerpj_2.1/cheerpjfy.py --deps $depsSepByColon $d
	fi

done



#:../lib/annotations-2.0.1.jar:../lib/guava-15.0.jar:../lib/javassist-3.19.0-GA.jar:../lib/reflections-0.9.10.jar

echo "Compiling jar into js..."
~/cheerpj_2.1/cheerpjfy.py --deps $depsSepByColon peachtree.jar
#../cheerpj/cheerpj_2.1/cheerpjfy.py --deps $depsSepByColon peachtree.jar





