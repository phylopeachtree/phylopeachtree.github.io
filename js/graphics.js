


CANCEL_GRAPHICS = false;
DOWNLOADING = false;
const SVGNS = "http://www.w3.org/2000/svg";

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
			
			//console.log("resizing");

			
			if (RESIZING) return;


			//console.log("rendering");

			RESIZING = true;
        		renderGraphics(function() { 
        			$("#svg").removeClass("resizing");
        			RESIZING = false; 
        		});

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


		
		
	
		//addLoader($("#upload_loading_div"));
		startLoading();
		$("#renderBtn").hide();
		
		



		// Get width and height
		var maxH = $( window ).height() - $("#mainHeader").offset().top - 300;
		var maxW = $("#main").offset().left + $("#main").width();
		//console.log("bottomY", maxH, maxW);
	

		// Generate the graphics objects
		callWasmFunction("initGraphics", [maxH, maxW, 0], function(initialVal){
		//cjCall("peachtree.options.OptionsAPI", "initGraphics").then(function(initialVal){
			
			SCROLLING = false;
			CANCEL_GRAPHICS = false;
			$("body").css("overflow", "hidden");
			
			
			var drawCanvas = false;

			
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


					// Prepare svg width/height
					var svg = $("#svg");
					var canvas = $("#canvas");
					var svgToDrawOn;
					var eleToDrawOn;
					
					// Clear canvas
					//const context = document.getElementById("canvas").getContext('2d');
					//context.clearRect(0, 0, canvas.width(), canvas.height());
					
					if (drawCanvas) {
						svgToDrawOn = document.createElementNS(SVGNS, "svg");
						var divID = "tmpSVG_DIV";
						var svgID = "tmpSVG";
						$("#" + divID).remove();
						$("body").append(`<div id="` + divID + `" style="display:none"> <svg id="` + svgID + `" style="font-family:'Courier New';dominant-baseline:middle;" xmlns="` + SVGNS + `"></svg></div>`);
						svgToDrawOn = $("#" + svgID);
						eleToDrawOn = canvas
						
						
						
						
						svgToDrawOn.height(height);
						svgToDrawOn.width(width);
						
						svg.hide(0);

						
					}else{
						svgToDrawOn = svg;
						eleToDrawOn = svg;
						svg.html("");
						svg.show(0);
					}
					
					
					
					
					

					// The thing to draw on
					eleToDrawOn.height(height);
					eleToDrawOn.width(width);

					svg.parent().height(height + padding);
					svg.parent().width(width + padding);
					
					
					
					
					$("#graphics_div").width(width + padding);


					var group1 = document.createElementNS(SVGNS, "g");
					var group2 = document.createElementNS(SVGNS, "g");
					var group3 = document.createElementNS(SVGNS, "g");
					var topGroup = document.createElementNS(SVGNS, "g");
					svgToDrawOn.append(group1);
					svgToDrawOn.append(group2);
					svgToDrawOn.append(group3);
					svgToDrawOn.append(topGroup);


					//console.log(initialVal);

					// Add the boundaries
					$(".draggableDivision").remove();
					for (var xboundary in initialVal.xboundaries){
						var val = initialVal.xboundaries[xboundary]["val"];
						var id = initialVal.xboundaries[xboundary]["id"]; 
						var contained = id != "width";
						//console.log("xb", id, val);
						createDraggableStick(eleToDrawOn, val, id, true, contained);
					}
					for (var yboundary in initialVal.yboundaries){
						
						var val = initialVal.yboundaries[yboundary]["val"];
						var id = initialVal.yboundaries[yboundary]["id"]; 
						var contained = id != "height";
						createDraggableStick(eleToDrawOn, val, id, false, contained);
					}
					
					
					// Add the scrollbars
					if (initialVal.scrolls != null){
						if (initialVal.scrolls.scrollY != null){
							let pos = initialVal.scrolls.scrollY;
							let len = initialVal.scrolls.scrollYLength;
							createScrollbar(eleToDrawOn, pos, len, "scrollY", true);
						}
						if (initialVal.scrolls.scrollX != null){
							let pos = initialVal.scrolls.scrollX;
							let len = initialVal.scrolls.scrollXLength;
							createScrollbar(eleToDrawOn, pos, len, "scrollX", false);
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
					plotNextObject(group1, group2, group3, 0, drawCanvas, resolve);


					
					
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







function plotNextObject(svgBtm, svgMid, svgTop, iteration = 0, drawCanvas, resolve = function() { } ){

	if (CANCEL_GRAPHICS){
		CANCEL_GRAPHICS = false;
		return;
	}
	
	
	// Comparing methods for runtime
	var useSVG = true;
	
	
	if (useSVG) {
		
		callWasmFunction("getGraphicsSVG", [], function(objects){
			
			
			//console.log(objects);
			
			
			if (objects.length == 0){
			
				//removeLoader($("#ctrl_loading_div"));
				stopLoading();
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
				
			}
			
			
			// Repeat
			plotNextObject(svgBtm, svgMid, svgTop, iteration + 1, drawCanvas, resolve)



			// Render bottom
			if (objects[0] != null){ 
				var eles = objects[0].join(" ");
				$(svgBtm)[0].innerHTML += eles;
				
			}
			
			// Render middle
			if (objects[1] != null){ 
				var eles = objects[1].join(" ");
				$(svgMid)[0].innerHTML += eles;
				
			}
			
			
			// Render top
			if (objects[2] != null){ 
				var eles = objects[2].join(" ");
				$(svgTop)[0].innerHTML += eles;

			}
			
		});

	}
	
	else {


		callWasmFunction("getGraphics", [], function(objects){
			
			

		
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

				// Draw canvas?
				if (drawCanvas){

					var svgText = $(svgBtm).parent().parent().html();
					//var svgText = $(svgBtm).parent()[1];
					//var svgText = document.getElementById("tmpSVG");
					
					// Draw canvas
					var canvas = document.getElementById("canvas");
					drawInlineSVG(canvas, svgText, function(x){
						//console.log(canvas.toDataURL());
						$(svgBtm).parent().parent().remove();
						resolve();
					});
	
				}else{
					resolve();
				}
				
				console.log("done");


				return;
				
			}else{

				
				// Repeat
				plotNextObject(svgBtm, svgMid, svgTop, iteration + 1, drawCanvas, resolve);
				
				var btmChunk = document.createElementNS(SVGNS, "g");
				var midChunk = document.createElementNS(SVGNS, "g");
				var topChunk = document.createElementNS(SVGNS, "g");
				
				
	


				// And render thes objects asynchronously...
				for (var i = 0; i < objects.length; i ++){
					var o = objects[i];
					drawSVGobj(btmChunk, midChunk, topChunk, o);

					
					//drawSVGobj(svgBtm, svgMid, svgTop, o);
				}
				
		
				// Add to the DOM at the very end
				svgBtm.append(btmChunk);
				svgMid.append(midChunk);
				svgTop.append(topChunk);
				

			}
			
			
				
		
		});

	}

}


/*
 * Convert an svg to a canvas
 */
function drawInlineSVG(canvas, rawSVG, resolve) {
	
	var ctx = canvas.getContext("2d");
	/*
	//console.log("drawing", rawSVG);
	
	let _settings = {
        svg: null,
        // Usually all SVG have transparency, so PNG is the way to go by default
        mimetype: "image/png",
        quality: 0.92,
        width: "auto",
        height: "auto",
        outputFormat: "base64"
    };
	//for (let key in settings) { _settings[key] = settings[key]; }
	
	
	
	
	 let svgXml = new XMLSerializer().serializeToString(svgNode);
        let svgBase64 = "data:image/svg+xml;base64," + btoa(svgXml);

        const image = new Image();

        image.onload = function(){
            

            // Define the canvas intrinsic size
            //canvas.width = finalWidth;
            //canvas.height = finalHeight;

            // Render image in the canvas
            ctx.drawImage(this, 0, 0, canvas.width, canvas.height);

            if(_settings.outputFormat == "blob"){
                // Fullfil and Return the Blob image
                canvas.toBlob(function(blob){
                    resolve(blob);
                }, _settings.mimetype, _settings.quality);
            }else{
                // Fullfil and Return the Base64 image
                resolve(canvas.toDataURL(_settings.mimetype, _settings.quality));
            }
        };

        // Load the SVG in Base64 to the image
        image.src = svgBase64;
	
	
	
	*/

    var svg = new Blob([rawSVG], {type:"image/svg+xml;charset=utf-8"}),
        domURL = self.URL || self.webkitURL || self,
		url = domURL.createObjectURL(svg);
		
	//url = "http://upload.wikimedia.org/wikipedia/commons/d/d2/Svg_example_square.svg";
		
	var img = new Image();


    img.onload = function () {
        ctx.drawImage(this, 0, 0, 100, 100);     
        domURL.revokeObjectURL(url);
        resolve(this);
    };

	console.log(url);

    img.src = url;
	
}


/*
 * Add an svg element to the page from its string
 */
function addSVGobj(svg, object){
	//console.log("appending", object);
	//var o = '<line stroke="black" stroke-dasharray="4,7" stroke-linecap="round" stroke-width="1.0" x1="355.0" x2="199.7600134313499" y1="188.5" y2="188.5" />';
	//svg.append(object);
	
	//document.getElementById("svg").innerHTML = o;
	
}


/*
 * Add an svg element to the page from its json
 */ 
function drawSVGobj(svgBtm, svgMid, svgTop, object){


	var type = object.ele;
	if (type == null) return;


	/*
	// Build string
	var str = "<" + type + " ";
	var value = "";
	
	// Set attributes
	for (var a in object){
		if (a == "ele") continue;
		if (a == "layer") continue;
		else if (a == "value") value += object[a];
		else if (a == "title") value += "<title>" + object[a] + "</title>";

		else str += (a.replace("_", "-")) + "='" + object[a] + "' ";
	}
	
	str += ">" + value + "</" + type + ">";
	
	var layer = object.layer == null || object.layer == 1 ? 1 : object.layer == 0 ? 0 : 2;
	
	result[layer] += str;


	//var svg = object.layer == null || object.layer == 1 ? svgMid : object.layer == 0 ? svgBtm : svgTop;
	//svg = $(svg)[0];
	
	
	//svg.innerHTML += str;
*/


	var newObj = document.createElementNS(SVGNS, type);


	// Set attributes
	for (var a in object){
		if (a == "ele") continue;
		if (a == "layer") continue;
		else if (a == "value") newObj.innerHTML += object[a];
		//else if (a == "bg") newObj.setAttribute("fill", object[a]);
		//else if (a == "col") newObj.setAttribute("color", object[a]);
		else if (a == "title") {
			var title = document.createElementNS(SVGNS, "title");
			title.innerHTML += object[a];
			newObj.append(title);
		}
		
		//if (a == "text_anchor") newObj.setAttribute("text-anchor", attr[a]);
		//else if (a == "alignment_baseline") newObj.setAttribute("alignment-baseline", attr[a]);
		//else if (a == "stroke_dasharray") newObj.setAttribute("stroke-dasharray", attr[a]);
		else newObj.setAttribute(a.replace("_", "-"), object[a]);
	}



	var svg = object.layer == null || object.layer == 1 ? svgMid : object.layer == 0 ? svgBtm : svgTop;
	svg.append(newObj);
	


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
	Download the svg
*/
function downloadImage(format = "svg"){


	isReadyToRender(function(ready){

		if (!ready || DOWNLOADING) return;

		//addLoader($("#ctrl_loading_div"));
		startLoading();

		var divID = "downloadSVG_DIV";
		var svgID = "downloadSVG";
		$("body").append(`<div id="` + divID + `" style="display:none"> <svg id="` + svgID + `" style="font-family:'Courier New';dominant-baseline:middle; "></svg></div>`);


		var resolve = function(){
			if (format == "svg"){
				saveSvg(document.getElementById(svgID), "peachtree");
			}
			
			else if (format == "png"){
				savePng(document.getElementById(svgID), "peachtree");
			}
			
			DOWNLOADING = false;
			//removeLoader($("#ctrl_loading_div"));
			stopLoading();
		};





		DOWNLOADING = true;

		// Generate the graphics objects
		callWasmFunction("initGraphics", [0, 0, 1], function(initialVal){
		//cjCall("peachtree.options.OptionsAPI", "initGraphics").then(function(initialVal){
			


			if (initialVal.err != null){
				alert(initialVal.err);
			}else{


				var drawCanvas = true;


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
				document.getElementById(svgID).setAttribute("viewBox", "0 0 " + width + " " + height);
				

				// Create top layer
				var group1 = document.createElementNS(SVGNS, "g");
				var group2 = document.createElementNS(SVGNS, "g");
				var group3 = document.createElementNS(SVGNS, "g");
				var topGroup = document.createElementNS(SVGNS, "g");
				svg.append(group1);
				svg.append(group2);
				svg.append(group3);
				svg.append(topGroup);

				console.log(initialVal);


				// Plot top level objects
				for (var i = 0; i < initialVal.objects.length; i ++){
					var o = initialVal.objects[i];
					drawSVGobj(group1, group2, group3, o, []);
				}


				// Plot json objects 1 chunk at a time
				plotNextObject(group1, group2, group3, 0, drawCanvas, resolve);

				
			}


		});
		


	});


}



/*
	Save the svg
*/
function saveSvg(svgEl, name) {
	
	
	name = name + ".svg";

	console.log("saving svg as", name);

	svgEl.setAttribute("xmlns", SVGNS);
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
	
	//svgEl.parentNode.removeChild(elem);
}



/*
	Save the svg as png
*/
function savePng(svgEl, name){
	
	
	const SCALE_BY = 4;
	svgEl = $(svgEl);
	
	name = name + ".png";
	
	console.log("saving png as", name);
	

	// Scale width and heigght. Apsect ratio will be preserved by viewBox
	svgEl.attr("xmlns", "http://www.w3.org/2000/svg");
	const w = parseFloat(svgEl.css("width"))*SCALE_BY;
	const h = parseFloat(svgEl.css("height"))*SCALE_BY;
	svgEl.attr("width", w);
	svgEl.attr("height", h);
	svgEl.removeClass("width");
	svgEl.removeClass("height");
	
	
	if (w*h > 2e8){
		//svgEl.parentNode.removeChild(elem);
		alert("Maximum png size exceeded!");
		return;
	}
	
	// Generate blob for png
	var svgStr = $(svgEl).prop('outerHTML');


	
	// Create canvas
	var canvas = document.createElement("canvas"); // create <canvas> element
    canvas.width = w;
    canvas.height = h;
	
    var context = canvas.getContext("2d");
	
    let image = new Image; // create <img> element
	
	
	// Download
	image.onload = function () {
		
		// White bg
		context.fillStyle = "white";
		context.fillRect(0, 0, w, h); 
		
		// Figure
		context.drawImage(image, 0, 0);
		
		var url = canvas.toDataURL("image/png");
		//console.log(url);
		var downloadLink = document.createElement("a");
		downloadLink.href = url;
		downloadLink.download = name;
		document.body.appendChild(downloadLink);
		downloadLink.click();
		document.body.removeChild(downloadLink);
		
		
		//svgEl.parentNode.removeChild(elem);
		
        
    }.bind(this);

    // btoa — binary string to ASCII (Base64-encoded)
    image.src = 'data:image/svg+xml;base64,' + btoa(unescape(encodeURIComponent(svgStr))); 
	

	
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
function scrollVertical(goingUp, bigScroll=false){
	
	
	let goingUpInt = goingUp ? -1 : 1;
	let bigScrollInt = bigScroll ? 1 : 0;

	clearTimeout($.data(this, 'timer'));
	$.data(this, 'timer', setTimeout(function() {
		
		
		
		if (SCROLLING) return;
		



		
		callWasmFunction("scrollABitVertical", [goingUpInt, bigScrollInt], function(result){
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
function scrollHorizontal(goingRight, bigScroll=false){
	
	
	let goingRightInt = goingRight ? 1 : -1;
	let bigScrollInt = bigScroll ? 1 : 0;

	clearTimeout($.data(this, 'timer'));
	$.data(this, 'timer', setTimeout(function() {
		
		
		
		if (SCROLLING) return;
		


		
		callWasmFunction("scrollABitHorizontal", [goingRightInt, bigScrollInt], function(result){
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
