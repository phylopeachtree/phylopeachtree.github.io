


function initGraphics(){
	
	
	
	
	
	
}


/*
	Draw if possible
*/
function renderGraphics(){
	
	isReadyToRender(function(ready){
		
		if (!ready) {
			console.log("Not ready to render!");
			return;
		}
		
		
		// Options
		renderOptions();
		
	

		// Generate the graphics objects
		cjCall("peachtree.options.OptionsAPI", "initGraphics").then(function(initialVal){


			
			var initialVal = JSON.parse(cjStringJavaToJs(initialVal));
			console.log("initialVal", initialVal)

			if (initialVal.err != null){
				alert(initialVal.err);
			}else{


				// Hide the upload menu
				$("#graphics_div").show(FADE_IN_TIME);
				$("#upload_div").hide(0);

				// Prepare svg width/height
				var svg = $("#svg");
				svg.html("");
				svg.height(initialVal.height);
				svg.width(initialVal.width);



				// Create top layer draggable
				var draggable = document.createElementNS('http://www.w3.org/2000/svg', "g");
				var mainGroup = document.createElementNS('http://www.w3.org/2000/svg', "g");
				svg.append(mainGroup);
				svg.append(draggable);
				draggable.setAttribute("id", "draggableG")



				// Sticks
				var taxonAlignmentBoundary = initialVal.taxon_alignment_boundary
				var stick = {ele: "line", x1: taxonAlignmentBoundary, y1: 0, x2: taxonAlignmentBoundary, y2: initialVal.height, stroke:"#666", stroke_width:1.5};
				drawSVGobj(draggable, stick);


				// Other meta info
				if (initialVal.ntaxa != null){
					$("#ntaxa_div").html("There are " + initialVal.ntaxa + " individuals");
				}else{
					$("#ntaxa_div").html("");
				}

				if (initialVal.nsites != null){
					$("#nsites_div").html("There are " + initialVal.nsites + " sites in the alignment");
				}else{
					$("#nsites_div").html("");
				}

				if (initialVal.nsitesdisplayed != null){
					$("#nsitesdisplayed_div").html("There are " + initialVal.nsitesdisplayed + " sites in the alignment being displayed");
				}else{
					$("#nsitesdisplayed_div").html("");
				}



				// Plot json objects 1 chunk at a time
				plotNextObject(mainGroup);
			}


		});
		
		
	})
	
	
}

/*
function makeDraggable(evt) {
	var svg = evt.target;
	svg.addEventListener('mousedown', startDrag);
	svg.addEventListener('mousemove', drag);
	svg.addEventListener('mouseup', endDrag);
	svg.addEventListener('mouseleave', endDrag);
	function startDrag(evt) {
	}
	function drag(evt) {
	}
	function endDrag(evt) {
	}
}
*/


function plotNextObject(svg, iteration = 0){

	cjCall("peachtree.options.OptionsAPI", "getGraphics").then(function(val){
								



			//console.log("graphics", cjStringJavaToJs(val));
			var objects = JSON.parse(cjStringJavaToJs(val));
			//console.log("iteration", iteration, "graphics", objects);


			if (objects.length == 0){
				return;
			}else{

				

				// Repeat
				plotNextObject(svg, iteration + 1)

				// And render thes objects asynchronously...
				for (var i = 0; i < objects.length; i ++){
					var o = objects[i];
					drawSVGobj(svg, o);
				}

			}
			
			
			
	
		});

}





function drawSVGobj(svg, object){


	//console.log("drawing", object);

	var type = object.ele;
	if (type == null) return;

	//console.log("attr", attr);
	var newObj = document.createElementNS('http://www.w3.org/2000/svg', type);


	// Set attributes
	for (var a in object){
		if (a == "ele") continue;
		else if (a == "value") newObj.innerHTML += object[a];
		//else if (a == "bg") newObj.setAttribute("fill", object[a]);
		//else if (a == "col") newObj.setAttribute("color", object[a]);
		else if (a == "title") {
			var title = document.createElementNS('http://www.w3.org/2000/svg', "title");
			title.innerHTML += object[a];
			newObj.append(title);
		}
		
		//if (a == "text_anchor") newObj.setAttribute("text-anchor", attr[a]);
		//else if (a == "alignment_baseline") newObj.setAttribute("alignment-baseline", attr[a]);
		//else if (a == "stroke_dasharray") newObj.setAttribute("stroke-dasharray", attr[a]);
		else newObj.setAttribute(a.replace("_", "-"), object[a]);
	}




	// Set some of the styles as attributes because safari and IE do not like styles for svgs
	//var styles = getComputedStyle(newObj);
	//if (styles.stroke != null) newObj.setAttribute("stroke", styles.stroke);
	//if (styles["stroke-width"] != null) newObj.setAttribute("stroke-width", styles["stroke-width"]);

	svg.append(newObj);
	

	
	return newObj;

}	











