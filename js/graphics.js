


CANCEL_GRAPHICS = false;


function initGraphics(){
	
	

	MOUSEWHEEL_DY = 0;
	SCROLLING = false;
	$(document).ready(function() {



		// No main page scroll when mouse is on svg
		$('#svg').mouseenter(function(e) {
			//console.log('no scroll');
			$("body").css("overflow", "hidden");
		});


		// Enable main page scroll when mouse is out of svg
		$('#svg').mouseleave(function(e) {
			//console.log('yes scroll');
			$("body").css("overflow", "auto");
		});

		 $('#svg').bind('mousewheel', function(e){



		 	let goingUp = e.originalEvent.wheelDelta/120 > 0;
		 	let goingUpInt = goingUp ? -1 : 1;

		 	MOUSEWHEEL_DY += goingUpInt;

			clearTimeout($.data(this, 'timer'));
			$.data(this, 'timer', setTimeout(function() {
				//console.log("Haven't scrolled in 50ms! dy = " + MOUSEWHEEL_DY);
				
				if (SCROLLING) return;

				let mw = MOUSEWHEEL_DY;
				MOUSEWHEEL_DY = 0;
				if (mw != 0){

					SCROLLING = true;
					callWasmFunction("scrollABit", [mw], function(val){
					//cjCall("peachtree.options.OptionsAPI", "scrollABit", mw).then(function(){
		        		renderGraphics(function() { SCROLLING = false; });
		        		//setTimeout(function() {
		        			//clearTimeout($.data(this, 'timer'));
	        			
		        		//}, 100);
		        		
		        	});
				}


			 //do something
			}, 40));




		 	

		 	/*
	        if(goingUp) {
	            console.log('scrolling up !');
	        }
	        else{
	            console.log('scrolling down !');
	        }
			*/





	    });


	});




	
}







/*
	Draw if possible
*/
function renderGraphics(resolve = function() {}){
	
	isReadyToRender(function(ready){
		

		
		if (!ready) {
			console.log("Not ready to render!");
			return;
		}
		
		
		CANCEL_GRAPHICS = true;
		
		
		// Options
		renderOptions();
		
		

	
		addLoader($("#upload_loading_div"));
		$("#renderBtn").hide();
	

		// Generate the graphics objects
		callWasmFunction("initGraphics", [], function(initialVal){
		//cjCall("peachtree.options.OptionsAPI", "initGraphics").then(function(initialVal){
			
			
			CANCEL_GRAPHICS = false;


			
			//var initialVal = JSON.parse(initialVal);
			console.log("initialVal", initialVal)

			if (initialVal.err != null){
				alert(initialVal.err);
			}else{


				// Hide the upload menu
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


					var width = initialVal.xboundaries.width;
					var height = initialVal.yboundaries.height;

					svg.html("");
					svg.height(height);
					svg.width(width);

					svg.parent().height(height + padding);
					svg.parent().width(width + padding);


					// Create top layer
					var mainGroup = document.createElementNS('http://www.w3.org/2000/svg', "g");
					var topGroup = document.createElementNS('http://www.w3.org/2000/svg', "g");
					svg.append(mainGroup);
					svg.append(topGroup);


					// Add the boundaries
					$(".draggableDivision").remove();
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
					
					
					// Add the scrollbars
					if (initialVal.scrolls != null){
						if (initialVal.scrolls.scrollY != null){
							let pos = initialVal.scrolls.scrollY;
							let len = initialVal.scrolls.scrollYLength;
							console.log("scrollY", pos, len);
							createScrollbar(svg, pos, len, "scrollY", true);
						}
						if (initialVal.scrolls.scrollX != null){
							let pos = initialVal.scrolls.scrollX;
							let len = initialVal.scrolls.scrollXLength;
							console.log("scrollX", pos, len);
							createScrollbar(svg, pos, len, "scrollX", false);
						}
					}






					// Other meta info
					$("#ntaxa_span").html(initialVal.ntaxa);
					$("#nsites_span").html(initialVal.nsites);
					$("#ntaxadisplayed_span").html(initialVal.ntaxadisplayed );
					$("#nsitesdisplayed_span").html(initialVal.nsitesdisplayed );
					$("#nuniqueseqs_span").html(initialVal.nuniqueseqs);
					updateSelectionCSS();


					// Plot top level objects
					for (var i = 0; i < initialVal.objects.length; i ++){
						var o = initialVal.objects[i];
						drawSVGobj(topGroup, o);
					}


					// Plot json objects 1 chunk at a time
					plotNextObject(mainGroup, 0, resolve);


					
					
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
			//newValue = newValue / svg.width();
 		}else{
 			newValue = $(this).offset().top - svg.offset().top;
			//newValue = newValue / svg.height();
 		}


 		console.log('new value', newValue);


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
 		console.log('new value', newValue);


		

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


function plotNextObject(svg, iteration = 0, resolve = function() { } ){

	if (CANCEL_GRAPHICS){
		CANCEL_GRAPHICS = false;
		return;
	}

	callWasmFunction("getGraphics", [], function(objects){
	//cjCall("peachtree.options.OptionsAPI", "getGraphics").then(function(val){
								



			//console.log("graphics", cjStringJavaToJs(val));
			//var objects = JSON.parse(cjStringJavaToJs(val));
			//console.log("iteration", iteration, "graphics", objects);


			if (objects.length == 0){
				
				removeLoader($("#ctrl_loading_div"));
				
				// Taxon selection
				$(svg).find(".taxon").click(function(){

					if (SHIFT_KEY_IS_DOWN){
						selectUpToTaxon($(this));
					}else{
						toggleTaxon($(this));
					}

					
				});


				// Node selection
				$(svg).find(".node").click(function(){
					flipSubtree($(this));
				});
				


				
				updateSelectionCSS();

				resolve();

				return;
				
			}else{

				// Repeat
				plotNextObject(svg, iteration + 1, resolve)

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





