


CANCEL_GRAPHICS = false;
DOWNLOADING = false;


function initGraphics(){
	
	
	SCROLLING = false;
	RESIZING = false;
	$(document).ready(function() {


		/*
		// No main page scroll when mouse is on svg
		$('#svg').mouseenter(function(e) {
			//console.log('no scroll');
			$("body").css("overflow", "hidden");
		});


		// Enable main page scroll when mouse is out of svg
		$('#svg').mouseleave(function(e) {
			//console.log('yes scroll');
			$("body").css("overflow", "hidden");
		});
		*/

		 $('#svg').bind('wheel', function(e){

		 	//console.log("when the wheels come down");
			
			

		 	let goingUp = e.originalEvent.wheelDelta/120 > 0;
		 	

			// Control mousewheel is zoom
			if (e.ctrlKey) {
				e.preventDefault();
				
				zoom(goingUp);
				
				
				
			}
			
			
			// Scroll
			else{
				
				
				scrollVertical(goingUp);

			}


	    });


	});





	// Redraw graphics on Window resize
	$(window).resize(function() {


		if (!$("#graphics_div").is(":visible")) return;

		//console.log("window resized");
		$("#svg").addClass("resizing");


		// Prevent resizes in short succession from triggering the event
		clearTimeout($.data(this, 'resizer'));
		$.data(this, 'resizer', setTimeout(function() {

			
			if (RESIZING) return;


			RESIZING = true;
				//callWasmFunction("scrollABit", [mw], function(val){
				//cjCall("peachtree.options.OptionsAPI", "scrollABit", mw).then(function(){
        		renderGraphics(function() { 
        			$("#svg").removeClass("resizing");
        			RESIZING = false; 
        		});
	        		//setTimeout(function() {
	        			//clearTimeout($.data(this, 'timer'));
        			
	        		//}, 100);
	        		
	        	//});


		 //do something
		}, 60));



	});





	
}







/*
	Draw if possible
*/
function renderGraphics(resolve = function() {}){
	

	if (DOWNLOADING) return;

	isReadyToRender(function(ready){
		
		
		if (!ready) {
			console.log("Not ready to render!");
			$("body").css("overflow", "visible");
			return;
		}



		
		CANCEL_GRAPHICS = true;

		
		
		// Options
		renderOptions();


		
		
	
		addLoader($("#upload_loading_div"));
		$("#renderBtn").hide();


		// Get width and height
		var maxH = $( window ).height() - $("#mainHeader").offset().top - 250;
		var maxW = $("#main").offset().left + $("#main").width();
		//console.log("bottomY", maxH, maxW);
	

		// Generate the graphics objects
		callWasmFunction("initGraphics", [maxH, maxW, 0], function(initialVal){
		//cjCall("peachtree.options.OptionsAPI", "initGraphics").then(function(initialVal){
			
			SCROLLING = false;
			CANCEL_GRAPHICS = false;
			$("body").css("overflow", "hidden");

			
			//var initialVal = JSON.parse(initialVal);
			//console.log("initialVal", initialVal)

			if (initialVal.err != null){
				alert(initialVal.err);
			}else{


				// Hide the upload menu
				$("#control_panel_div").show(0);
				$("#graphics_div").show(0);
				$("#upload_div").hide(0);

				removeLoader($("#upload_loading_div"));
				$("#renderBtn").show();
				
				
				// Search bar
				populateTaxonSearchBar();
				
				// Async
				setTimeout(function() {

					// Prepare svg width/height
					var svg = $("#svg");
					const padding = 0;


					var width, height;
					for (var xboundary in initialVal.xboundaries){
						var id = initialVal.xboundaries[xboundary]["id"]; 
						if (id == "width") width = initialVal.xboundaries[xboundary]["val"]; 
					}
					for (var yboundary in initialVal.yboundaries){
						var id = initialVal.yboundaries[yboundary]["id"]; 
						if (id == "height") height = initialVal.yboundaries[yboundary]["val"]; 
					}


					svg.html("");
					svg.height(height);
					svg.width(width);

					svg.parent().height(height + padding);
					svg.parent().width(width + padding);
					$("#graphics_div").width(width + padding);


					var group1 = document.createElementNS('http://www.w3.org/2000/svg', "g");
					var group2 = document.createElementNS('http://www.w3.org/2000/svg', "g");
					var group3 = document.createElementNS('http://www.w3.org/2000/svg', "g");
					var topGroup = document.createElementNS('http://www.w3.org/2000/svg', "g");
					svg.append(group1);
					svg.append(group2);
					svg.append(group3);
					svg.append(topGroup);


					//console.log(initialVal);

					// Add the boundaries
					$(".draggableDivision").remove();
					for (var xboundary in initialVal.xboundaries){
						var val = initialVal.xboundaries[xboundary]["val"];
						var id = initialVal.xboundaries[xboundary]["id"]; 
						var contained = id != "width";
						//console.log("xb", id, val);
						createDraggableStick(svg, val, id, true, contained);
					}
					for (var yboundary in initialVal.yboundaries){
						
						var val = initialVal.yboundaries[yboundary]["val"];
						var id = initialVal.yboundaries[yboundary]["id"]; 
						var contained = id != "height";
						createDraggableStick(svg, val, id, false, contained);
					}
					
					
					// Add the scrollbars
					if (initialVal.scrolls != null){
						if (initialVal.scrolls.scrollY != null){
							let pos = initialVal.scrolls.scrollY;
							let len = initialVal.scrolls.scrollYLength;
							createScrollbar(svg, pos, len, "scrollY", true);
						}
						if (initialVal.scrolls.scrollX != null){
							let pos = initialVal.scrolls.scrollX;
							let len = initialVal.scrolls.scrollXLength;
							createScrollbar(svg, pos, len, "scrollX", false);
						}
					}



		


					// Other meta info
					$("#ntaxa_span").html(initialVal.ntaxa);
					$("#ntaxadisplayed_span").html(initialVal.ntaxadisplayed );
					if (initialVal.nsites != null){
						$("#nsites_span").html(initialVal.nsites);
						$("#nsitesdisplayed_span").html(initialVal.nsitesdisplayed );
						$("#nuniqueseqs_span").html(initialVal.nuniqueseqs);
						$(".alignmentSummary").show(50);
					}else{
						$(".alignmentSummary").hide(0);
					}
					updateSelectionCSS();


					// Plot top level objects
					for (var i = 0; i < initialVal.objects.length; i ++){
						var o = initialVal.objects[i];
						drawSVGobj(group1, group2, group3, o);
					}


					// Plot json objects 1 chunk at a time
					plotNextObject(group1, group2, group3, 0, resolve);


					
					
				}, 1);
			}


		});
		
		
	})
	
	
}




/*
	Create a scrollbar
*/
function createScrollbar(svg, pos, scrollLength, id, vertical=true){
	
	$("#" + id).remove();
	const axis = vertical ? "y" : "x";
	const scrollWidth = 7;
	const padding = 3;
	
	
	let left, top, width, height;
	
	// Vertical 
	if (vertical) {
		
		left = svg.offset().left + padding;
		top = svg.offset().top + pos;
		width = scrollWidth + padding;
		height = scrollLength;
	}

	// Horizontal
	else{
		
		left = svg.offset().left + pos;
		top = svg.offset().top + padding;
		width = scrollLength;
		height = scrollWidth + padding;
	}
	
	// Create thumb
	var thumb = $('<canvas/>',{'id':id, 'class':'draggableDivision scroll', title:'Scroll'}).width(width).height(height);
	thumb.offset({left: left, top: top});
	$("#graphics_div").append(thumb)
	
	
	var canvas = document.getElementById(id);
	var ctx = canvas.getContext('2d');
	//ctx.fillStyle = "#29465b";
	ctx.fillStyle = "#5ca904";
	
	//ctx.fillRect(0, 0, canvas.width, canvas.height);
	roundRect(ctx, 0, 0, canvas.width, canvas.height, 300/scrollLength, "#5ca904");
	
	
	
	// Draggable options
	const options = {axis: axis}
	options.containment = svg.parent();
	options.start = function(){
			svg.addClass("resizing");
	};
	options.stop = function(event, ui){
		
		svg.removeClass("resizing");
 		var newValue;
 		if (!vertical) {
 			newValue = $(this).offset().left - svg.offset().left;


 			var div2 = $("#division2").offset().left - svg.offset().left;
 			var tx = $(this).offset().left - svg.offset().left;
 			tx = tx - div2;
 			if (tx < 0) tx = 0;
 			var vw = svg.width() - div2;
 			var tw = $(this).width();

 			newValue = tx / (vw-tw);


			//newValue = newValue / svg.width();
 		}else{

			//console.log("scrolly", $(this).offset().top, $(this).height(), svg.offset().top, svg.height());

 			//newValue = $(this).offset().top - svg.offset().top;

 			var ty = $(this).offset().top - svg.offset().top;
 			var vh = svg.height();
 			var th = $(this).height();
 			
			newValue = ty / (vh-th);

 		}




 		// Set options
		setOptionToVal(id, newValue);

 		
	} 


	// Make it draggable
	$("#" + id).draggable(options);




	
	
}


/*
	Create a draggable division within the svg
*/
function createDraggableStick(svg, pos, id, xAxis, contained){


	$("#" + id).remove();
	const axis = xAxis ? "x" : "y";
	const divisionWidth = 2;
	const padding = 2;



	let left, top, height, width;


	// Vertical 
	if (xAxis) {
		left = svg.offset().left + pos;
		top = svg.offset().top;
		width = divisionWidth;
		height = svg.height() + padding;
	}

	// Horizontal
	else{
		left = svg.offset().left;
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
	ctx.fillStyle = "#29465b";
	roundRect(ctx, 0, 0, canvas.width, canvas.height, 20, "#29465b");


	// Draggable options
	const options = {axis: axis}
	if (contained) options.containment = svg.parent();
	options.start = function(){
			svg.addClass("resizing");
	};
	options.stop = function(event, ui){
		
		svg.removeClass("resizing");
 		var newValue;
 		if (xAxis) {
 			newValue = $(this).offset().left - svg.offset().left;
 		}else{
 			newValue = $(this).offset().top - svg.offset().top;
 		}

 		
 		if (contained && xAxis) newValue = newValue / svg.width();
 		if (contained && !xAxis) newValue = newValue / svg.height();


		

 		// Set options
		setOptionToVal(id, newValue);


 		
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


function plotNextObject(svgBtm, svgMid, svgTop, iteration = 0, resolve = function() { } ){

	if (CANCEL_GRAPHICS){
		CANCEL_GRAPHICS = false;
		return;
	}
	
	
	/*
	callWasmFunction("getGraphicsSVG", [], function(objects){
		
		
		if (objects.svg != null){
			
			
			
		}
		
	});
	
	
	return;
*/

	callWasmFunction("getGraphics", [], function(objects){
	//cjCall("peachtree.options.OptionsAPI", "getGraphics").then(function(val){
								



			//console.log("graphics", cjStringJavaToJs(val));
			//var objects = JSON.parse(cjStringJavaToJs(val));
			//console.log("iteration", iteration, "graphics", objects);


			if (objects.length == 0){
				
				removeLoader($("#ctrl_loading_div"));
				$("#svg").removeClass("resizing");
				
				// Taxon selection
				$(svgBtm).parent().find(".taxon").click(function(){

					if (SHIFT_KEY_IS_DOWN){
						selectUpToTaxon($(this));
					}else{
						toggleTaxon($(this));
					}

					
				});


				// Node selection
				$(svgBtm).parent().find(".node").click(function(){
					flipSubtree($(this));
				});
				


				
				updateSelectionCSS();
				
				resolve();

				return;
				
			}else{

				// Repeat
				plotNextObject(svgBtm, svgMid, svgTop, iteration + 1, resolve)

				// And render thes objects asynchronously...
				for (var i = 0; i < objects.length; i ++){
					var o = objects[i];
					drawSVGobj(svgBtm, svgMid, svgTop, o);
				}

			}
			
			
			
	
		});

}





function drawSVGobj(svgBtm, svgMid, svgTop, object){


	//console.log("drawing", object);

	var type = object.ele;
	if (type == null) return;


	var svg = object.layer == null || object.layer == 1 ? svgMid : object.layer == 0 ? svgBtm : svgTop;

	//console.log("attr", attr);
	var newObj = document.createElementNS('http://www.w3.org/2000/svg', type);


	// Set attributes
	for (var a in object){
		if (a == "ele") continue;
		if (a == "layer") continue;
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




/*
	Rounded rectangle on a canvas
	https://stackoverflow.com/questions/1255512/how-to-draw-a-rounded-rectangle-on-html-canvas
*/
function roundRect(ctx, x, y, width, height, radius, fill, stroke) {
  if (typeof stroke === 'undefined') {
    stroke = true;
  }
  if (typeof radius === 'undefined') {
    radius = 5;
  }
  if (typeof radius === 'number') {
    radius = {tl: radius, tr: radius, br: radius, bl: radius};
  } else {
    var defaultRadius = {tl: 0, tr: 0, br: 0, bl: 0};
    for (var side in defaultRadius) {
      radius[side] = radius[side] || defaultRadius[side];
    }
  }
  ctx.beginPath();
  ctx.moveTo(x + radius.tl, y);
  ctx.lineTo(x + width - radius.tr, y);
  ctx.quadraticCurveTo(x + width, y, x + width, y + radius.tr);
  ctx.lineTo(x + width, y + height - radius.br);
  ctx.quadraticCurveTo(x + width, y + height, x + width - radius.br, y + height);
  ctx.lineTo(x + radius.bl, y + height);
  ctx.quadraticCurveTo(x, y + height, x, y + height - radius.bl);
  ctx.lineTo(x, y + radius.tl);
  ctx.quadraticCurveTo(x, y, x + radius.tl, y);
  ctx.closePath();
  if (fill) {
    ctx.fill();
  }
  if (stroke) {
    ctx.stroke();
  }

}




/*
	Download the alignment
*/
function downloadAlignment(){
	callWasmFunction("downloadAlignment", [], function(result){
		download("peachtree.fasta", result.contents);
	});
}



/*
	Download the samples
*/
function downloadSamples(){
	callWasmFunction("downloadSamples", [], function(result){
		download("peachtree.csv", result.contents);
	});
}


/*
	Download the tree
*/
function downloadTree(){
	callWasmFunction("downloadTree", [], function(result){
		download("peachtree.nexus", result.contents);
	});
}



/*
	Download the svg
*/
function downloadImage(){


	isReadyToRender(function(ready){

		if (!ready || DOWNLOADING) return;

		addLoader($("#ctrl_loading_div"));

		var divID = "downloadSVG_DIV";
		var svgID = "downloadSVG";
		$("body").append(`<div id="` + divID + `" style="display:none"> <svg id="` + svgID + `" style="font-family:'Courier New';dominant-baseline:middle; "></svg></div>`);


		var resolve = function(){
			saveSvg(document.getElementById(svgID), "peachtree.svg");
			DOWNLOADING = false;
			removeLoader($("#ctrl_loading_div"));
		};





		DOWNLOADING = true;

		// Generate the graphics objects
		callWasmFunction("initGraphics", [0, 0, 1], function(initialVal){
		//cjCall("peachtree.options.OptionsAPI", "initGraphics").then(function(initialVal){
			


			if (initialVal.err != null){
				alert(initialVal.err);
			}else{




				// Prepare svg width/height
				var svg = $("#" + svgID);
				const padding = 0;


				var width, height;
				for (var xboundary in initialVal.xboundaries){
					var id = initialVal.xboundaries[xboundary]["id"]; 
					if (id == "width") width = initialVal.xboundaries[xboundary]["val"]; 
				}
				for (var yboundary in initialVal.yboundaries){
					var id = initialVal.yboundaries[yboundary]["id"]; 
					if (id == "height") height = initialVal.yboundaries[yboundary]["val"]; 
				}


				svg.html("");
				svg.height(height);
				svg.width(width);

				// Create top layer
				var group1 = document.createElementNS('http://www.w3.org/2000/svg', "g");
				var group2 = document.createElementNS('http://www.w3.org/2000/svg', "g");
				var group3 = document.createElementNS('http://www.w3.org/2000/svg', "g");
				var topGroup = document.createElementNS('http://www.w3.org/2000/svg', "g");
				svg.append(group1);
				svg.append(group2);
				svg.append(group3);
				svg.append(topGroup);

				console.log(initialVal);


				// Plot top level objects
				for (var i = 0; i < initialVal.objects.length; i ++){
					var o = initialVal.objects[i];
					drawSVGobj(group1, group2, group3, o);
				}


				// Plot json objects 1 chunk at a time
				plotNextObject(group1, group2, group3, 0, resolve);

				
			}


		});
		


	});


}



/*
	Save the svg
*/
function saveSvg(svgEl, name) {

	console.log("saving as", name);

	svgEl.setAttribute("xmlns", "http://www.w3.org/2000/svg");
	var svgData = svgEl.outerHTML;
	var preface = '<?xml version="1.0" standalone="no"?>\r\n';
	var svgBlob = new Blob([preface, svgData], {type:"image/svg+xml;charset=utf-8"});
	var svgUrl = URL.createObjectURL(svgBlob);
	var downloadLink = document.createElement("a");
	downloadLink.href = svgUrl;
	downloadLink.download = name;
	document.body.appendChild(downloadLink);
	downloadLink.click();
	document.body.removeChild(downloadLink);
}



/*
	Zoom in/out
*/
function zoom(zoomIn){
	
	
	clearTimeout($.data(this, 'timer'));
	$.data(this, 'timer', setTimeout(function() {

		
		
		if (SCROLLING) return;


		
		callWasmFunction("zoomABit", [(zoomIn ? 1 : -1)], function(result){
			if (result.changed) {
				SCROLLING = true;
				CANCEL_GRAPHICS = true;
				$("#svg").addClass("resizing");
				renderGraphics(function() {  });
			}
		});
		


	 //do something
	}, 40));
	
}


/*
 * Go to next / prev tree
*/
function moveTree(plus){
	
	callWasmFunction("moveTree", [plus], function(result){
		//if (result.changed) {
			CANCEL_GRAPHICS = true;
			$("#svg").addClass("resizing");
			renderGraphics(function() {  });
		//}
	});
	
}

/*
	Scroll up/down slightly
*/
function scrollVertical(goingUp){
	
	
	let goingUpInt = goingUp ? -1 : 1;

	clearTimeout($.data(this, 'timer'));
	$.data(this, 'timer', setTimeout(function() {
		
		
		
		if (SCROLLING) return;
		



		
		callWasmFunction("scrollABitVertical", [goingUpInt], function(result){
			if (result.changed) {
				SCROLLING = true;
				CANCEL_GRAPHICS = true;
				$("#svg").addClass("resizing");
				renderGraphics(function() {  });
			}
		});
		

	}, 40));


	
}


/*
	Scroll left/right slightly
*/
function scrollHorizontal(goingRight){
	
	
	let goingRightInt = goingRight ? 1 : -1;

	clearTimeout($.data(this, 'timer'));
	$.data(this, 'timer', setTimeout(function() {
		
		
		
		if (SCROLLING) return;
		


		
		callWasmFunction("scrollABitHorizontal", [goingRightInt], function(result){
			if (result.changed) {
				SCROLLING = true;
				CANCEL_GRAPHICS = true;
				$("#svg").addClass("resizing");
				renderGraphics(function() {  });
			}
		});
		

	}, 40));
	
}



/*
* Download a textfile
*/
function download(filename, text) {
	
	if (filename == "" || filename == null || text == "" || text == null) return;
	
	var element = document.createElement('a');
	element.setAttribute('href', 'data:text/plain;charset=utf-8,' + encodeURIComponent(text));
	element.setAttribute('download', filename);
	element.style.display = 'none';
	document.body.appendChild(element);
	element.click();
	document.body.removeChild(element);
	
}
