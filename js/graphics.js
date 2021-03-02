


CANCEL_GRAPHICS = false;


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
				const padding = 0;


				var width = initialVal.xboundaries.width;
				var height = initialVal.yboundaries.height;

				svg.html("");
				svg.height(height);
				svg.width(width);

				svg.parent().height(height + padding);
				svg.parent().width(width + padding);


				// Create top layer draggable
				var mainGroup = document.createElementNS('http://www.w3.org/2000/svg', "g");
				svg.append(mainGroup);


				// Add the boundaries
				for (var xboundary in initialVal.xboundaries){
					console.log("x", xboundary);
					var contained = xboundary != "width";
					createDraggableStick(svg, initialVal.xboundaries[xboundary], xboundary, true, contained);
				}
				for (var yboundary in initialVal.yboundaries){
					console.log("y", yboundary);
					var contained = yboundary != "height";
					createDraggableStick(svg, initialVal.yboundaries[yboundary], yboundary, false, contained);
				}






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
	Create a draggable division within the svg
*/
function createDraggableStick(svg, pos, id, xAxis, contained){


	$("#" + id).remove();
	const axis = xAxis ? "x" : "y";
	const divisionWidth = 2;
	const padding = 3;



	let left, top, height, width;


	// Vertical 
	if (xAxis) {
		left = svg.offset().left + pos;
		top = svg.offset().top - padding;
		width = divisionWidth;
		height = svg.height() + padding;
	}

	// Horizontal
	else{
		left = svg.offset().left - padding;
		top = svg.offset().top + pos;
		width = svg.width() +  padding;
		height = divisionWidth;
	}
	

	var stick = $('<canvas/>',{'id':id, 'class':'draggableDivision ' + axis}).width(width).height(height);
	stick.offset({left: left, top: top});

	$("#graphics_div").append(stick)
	

	// Get the canvas
	var canvas = document.getElementById(id);
	var ctx = canvas.getContext('2d');
	ctx.fillStyle = '#aaaaaa';
	ctx.fillRect(0, 0, canvas.width, canvas.height);


	// Draggable options
	const options = {axis: axis}
	if (contained) options.cotainment = svg.parent();
	options.start = function(){
			svg.addClass("resizing");
	};
	options.stop = function(event, ui){
		CANCEL_GRAPHICS = true;
		svg.removeClass("resizing");
 		var newValue;
 		if (xAxis) {
 			newValue = $(this).offset().left - svg.offset().left;
 		}else{
 			newValue = $(this).offset().top - svg.offset().top;
 		}

 		
 		if (contained && xAxis) newValue = newValue / svg.width();
 		if (contained && !xAxis) newValue = newValue / svg.height();
 		console.log('new value', newValue);

 		// Set options
		cjCall("peachtree.options.OptionsAPI", "setOption", id, newValue).then(function(val){
			


			//console.log("done", val);
			CANCEL_GRAPHICS = false;
			renderGraphics();
			
		});

 		
	} 


	// Make it draggable
	$("#" + id).draggable(options);




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

	if (CANCEL_GRAPHICS){
		CANCEL_GRAPHICS = false;
		return;
	}

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











