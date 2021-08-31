


function initUtil(){
	

	$("#control_panel_div").hide(0);
	$("#graphics_div").hide(0);
	$("#upload_div").show(0);
	BUILDING_TREE = false;

	// Is shift key down right now
	SHIFT_KEY_IS_DOWN = false;

	window.onkeyup = function(e) { 
		if (SHIFT_KEY_IS_DOWN && e.keyCode == 16) {
			//console.log("shift up");
			SHIFT_KEY_IS_DOWN = false;
		}
	}
	window.onkeydown = function(e) { 
		if (!SHIFT_KEY_IS_DOWN && e.keyCode == 16) {
			//console.log("shift down");
			SHIFT_KEY_IS_DOWN = true;
		}
	}
		

	// Alignment uploader
	var alignmentDropzone = new Dropzone("#aln_upload", { url: "/file/post"});
	alignmentDropzone.on("addedfile", function(file) {
		
		
		var reader = new FileReader();


	

		// Closure to capture the file information.
		reader.onload = (function(theFile) {
			
			addLoader($("#aln_upload_title"));
			$("#aln_upload .usermsg").html("");
			
			return function(e) {
				
				try {
				
					
					//parseSpeciesTree(e, util_file);
					if (e == null || e.target == null || e.target.result == null){
						throw {message: "Cannot open file"};
					}
					var contents = e.target.result;
					callWasmFunction("uploadAlignment", [contents], function(val){
					//cjCall("peachtree.aln.AlignmentAPI", "uploadAlignment", contents).then(function(val){
						
						console.log("val", val);

						//val = JSON.parse(val);
						

						
						try {
							//var val = JSON.parse(cjStringJavaToJs(val));
							console.log(val);
							if (val.err != null) {
								throw {message: val.err};
							}
						}catch(err){
							 return plotUploadErrorMsg(err, "#aln_upload");
							 
						}
						
				
						return plotUploadSuccessMsg(file.name, val.time, "#aln_upload");
						
					}, true);
					
					
				}catch(err){
					return plotUploadErrorMsg(err, "#aln_upload");
				}
				
			
				
				
			};

		})(file);

		reader.readAsText(file);
		
		
		

	});



	// Tree uploader
	var treeDropzone = new Dropzone("#phy_upload", { url: "/file/post"});
	treeDropzone.on("addedfile", function(file) {
		
		
		var reader = new FileReader();


	

		// Closure to capture the file information.
		reader.onload = (function(theFile) {
			
			addLoader($("#phy_upload_title"));
			$("#phy_upload .usermsg").html("");
			
			return function(e) {
				
				try {
				
					
					//parseSpeciesTree(e, util_file);
					if (e == null || e.target == null || e.target.result == null){
						throw {message: "Cannot open file"};
					}
					var contents = e.target.result;
					callWasmFunction("uploadTree", [contents], function(val){
					//cjCall("peachtree.phy.PhylogenyAPI", "uploadTree", contents).then(function(val){
						
						try {
							//var val = JSON.parse(cjStringJavaToJs(val));
							console.log(val);
							if (val.err != null) {
								throw {message: val.err};
							}
						}catch(err){
							 return plotUploadErrorMsg(err, "#phy_upload");
							 
						}
						
				
						return plotUploadSuccessMsg(file.name, val.time, "#phy_upload");
					}, true);
					
					
				}catch(err){
					return plotUploadErrorMsg(err, "#phy_upload");
				}
				
			
				
				
			};

		})(file);

		reader.readAsText(file);
		
		
		

	});
	
	


	// Epi uploader
	var alignmentDropzone = new Dropzone("#epi_upload", { url: "/file/post"});
	alignmentDropzone.on("addedfile", function(file) {
		
		
		var reader = new FileReader();


		// Closure to capture the file information.
		reader.onload = (function(theFile) {
			
			addLoader($("#epi_upload_title"));
			$("#epi_upload .usermsg").html("");

			
			return function(e) {
				
				try {



				
					
					//parseSpeciesTree(e, util_file);
					if (e == null || e.target == null || e.target.result == null){
						throw {message: "Cannot open file"};
					}
					var contents = e.target.result;
					callWasmFunction("uploadEpi", [contents], function(val){
					//cjCall("peachtree.epi.EpiAPI", "uploadEpi", contents).then(function(val){
						
						try {
							//var val = JSON.parse(cjStringJavaToJs(val));
							console.log(val);
							if (val.err != null) {
								throw {message: val.err};
							}
						}catch(err){
							 return plotUploadErrorMsg(err, "#epi_upload");
							 
						}
						
				
						return plotUploadSuccessMsg(file.name, val.time, "#epi_upload");
					}, true);
					
					
				}catch(err){
					return plotUploadErrorMsg(err, "#epi_upload");
				}
				
			
				
				
			};

		})(file);

		reader.readAsText(file);
		
		
		

	});



	
	
}



/*
	Return to the upload screen
*/
function reupload(){

	$("#control_panel_div").hide(0);
	$("#graphics_div").hide(0);
	$("#upload_div").show(0);
	$("#options_tabs").html("");
	$("#slideIns").html("");
	
}



/*
	Error message for uploader
*/
function plotUploadErrorMsg(err, uploadSelector){
	console.log("caught", err);
	$(uploadSelector + " .usermsg").html("<b>Error: </b>" + err.message);
	removeLoader($(uploadSelector + "_title"));
	return false;
}


/*
	Clear error message on success
*/
function plotUploadSuccessMsg(filename, time, uploadSelector){

	var html = "";
	if (time == "0" || time == 0) {
		html = filename + " successfully parsed in <1s!";
	}else{
		html = filename + " successfully parsed in " + Math.round(time*10)/10 + "s!";
	}

	html += "<span selector='" + uploadSelector + "'' onclick='removeUpload(this)' class='removeUploadBtn' title='Remove uploaded file'> &#10005; </span>"

	$(uploadSelector + " .usermsg").html(html);

	
	updateRenderBtn();
	removeLoader($(uploadSelector + "_title"));
	return true;
}



/*
	Remove an uploaded file
*/
function removeUpload(ele){


	var resolve = function(x){
		$(id + " .usermsg").html("");
		updateRenderBtn();
	}

	var id = $(ele).attr("selector");
	console.log(id);

	if (id == "#aln_upload"){
		callWasmFunction("removeAlignmentUpload", [], resolve);
	}else if (id == "#phy_upload"){
		callWasmFunction("removeTreeUpload", [], resolve);
	}else if (id == "#epi_upload"){
		callWasmFunction("removeEpiUpload", [], resolve);
	}




}


/*
	Adds a loading icon
*/
function addLoader(ele, large=false){
	ele.parent().addClass("loading");
	if (large){
		ele.append(`<div title="Loading..." class="loader large"></div>`);
	}else{
		ele.append(`<div title="Loading..." class="loader"></div>`);
		$("body").addClass("loading");
	}
	
}

function removeLoader(ele){
	ele.parent().removeClass("loading");
	ele.find(" .loader").remove();
	$("body").removeClass("loading");
}


/*
	Display all user options
*/
function renderOptions(){
	
	callWasmFunction("getOptions", [], function(options){
	//cjCall("peachtree.options.OptionsAPI", "getOptions").then(function(options){
						
			
		//var options = JSON.parse(cjStringJavaToJs(options));
		//console.log("options", options);


		var optionsDiv = $("#options_div");
		var optionsTabs = $("#options_tabs");
		var slideIns = $("#slideIns");
		optionsTabs.html("");
		slideIns.html("");

		if (options.err != null){
			alert(options.err);
			return;
		}
		
		

		// Get the unique list of sections
		var sections = [];
		for (var i = 0; i < options.length; i ++){
			let opt = options[i];
			if (opt.hide) continue;
			let s = opt.section;
			if (!sections.includes(s)) sections.push(s);

		}
		//console.log("sections", sections);

		// Create one tab per section
		for (let i = 0; i < sections.length; i++){


			var slideInID = "options_" + sections[i];

			// Tab
			optionsTabs.append("<li onClick='toggleTab(this)' title='Open/close tab' slide='" + slideInID + "'>" + sections[i] + "</li>")


			// Slide in html
			var slideHTML = `<div class="optionsSlideIn" style="display:none">
				<div>
					<h2>
					` + sections[i] + ` settings
					</h2>
					<div></div>
				</div>
			</div>`;
			
			
			var div = $(slideHTML)
			div.attr("id", slideInID);
			slideIns.append(div)
			
		
			

			// Get the options of this section
			for (let o = 0; o < options.length; o ++){
				let opt = options[o];
				if (opt.hide) continue;
				if (opt.section == sections[i]){
					
					
					if (opt.type == "NumericalOption"){
						
						// Range slider html
						
						let optionsHTML = `
						<div class="optionsBox" title="` + opt.longTitle + `">
							
							<div class="slidecontainer">
								 <input onChange="setOptionFromEle(this)" type="range" min="` + opt.min + `" max="` + opt.max + `" value="` + opt.value + `" step="` + opt.step + `" class="slider" var="` + opt.name + `">
							</div>
							` + opt.title + ` (` + opt.value + `)
						</div>`;
						div.children("div").children("div").append(optionsHTML);
						
					}
					
					
					else if (opt.type == "BooleanOption"){
						
						
						// Switch html
						let optionsHTML = `
						<div class="optionsBox"  title="` + opt.longTitle + `">
							<div>
								<label class="switch">
									<input onChange="setOptionFromEle(this)" var="` + opt.name + `" type="checkbox">
									<span class="sliderboolean"></span>
								</label>
							</div>
							` + opt.title + `
						</div>`;
						div.children("div").children("div").append(optionsHTML);
						div.find(`input[var="` + opt.name + `"]`).prop("checked", opt.value);
						
					}
					
					
					else if (opt.type == "DiscreteOption"){
						
						
						
						
						// Dropdown html
						let optionsHTML = `
						<div class="optionsBox"  title="` + opt.longTitle + `">
							
							<div class="dropdown">
								  <select onChange="setOptionFromEle(this)" var="` + opt.name + `">
									</select>
							</div>
							` + opt.title + `
						</div>`;
						div.children("div").children("div").append(optionsHTML);
						
						
						
						for (let j = 0; j < opt.domain.length; j ++){
							let dropdownOptionsHTML = `<option value="` + opt.domain[j] + `">` + opt.domain[j] + `</option>`;
							div.find(`select[var="` + opt.name + `"]`).append(dropdownOptionsHTML);
							if (opt.domain[j] == opt.value) div.find(`select[var="` + opt.name + `"]`).val(opt.domain[j]);
						}
						
						
						
					}


					else if (opt.type == "ColourOption"){
						
						
						
						
						// Dropdown html
						let pickerHTML = $(`<div class="optionsBox colourPicker" title="` + opt.longTitle + `"></div>`);
						
						
						for (let j = 0; j < opt.colours.length; j ++){
							let colHTML = $(`<span var="` + opt.name + `" value="` + opt.colours[j] + `" style="background-color:` + opt.colours[j] + 
												`" onclick="setOptionFromEle(this)" title="Select colour"></span>`);
							if (opt.colours[j] == opt.value) colHTML.addClass("selected");
							pickerHTML.append(colHTML);
						}
						pickerHTML.append("<br>" + opt.title);

						

						div.children("div").children("div").append(pickerHTML);
						
						
						
					}
		
					
				}
			}
			
			
			

	
			

		}

		
		//.html(JSON.stringify(options));
		//addLoader($("#buildTreeBtn").parent());
		
		
		callWasmFunction("treeIsReady", [], function(result){
			
			if (result.ready) {
				$("#btn_clade").show(100);
				$("#downloadTreeBtn").show(100);
			}
			else {
				$("#btn_clade").hide(0);
				$("#downloadTreeBtn").hide(0);
			}
			
		});
		
		
		callWasmFunction("alignmentIsMock", [], function(result){
			
			if (!result.mock) {
				$("#buildTreeBtn").show(100);
				$("#downloadAlignmentBtn").show(0);
			}
			else {
				$("#buildTreeBtn").hide(0);
				$("#downloadAlignmentBtn").hide(0);
			}
			
			
			
		});
		
		
				
		callWasmFunction("transmissionTreeCanBeReordered", [], function(result){
			
			if (result.ready) {
				$("#reorderTreeBtn").show(100);
			}
			else {
				$("#reorderTreeBtn").hide(0);
			}
			
		});
		
		
		
		
		

		
		
		
				
	});
	
}


/*
	Sets the value of this option from the element
*/
function setOptionFromEle(ele){
	
	let id = $(ele).attr("var");
	let val = 0;
	if ($(ele).prop('nodeName') == "INPUT" && $(ele).prop("type") == "checkbox"){
		val = $(ele).prop("checked");
	}else if($(ele).prop('nodeName') == "SPAN"){
		val = $(ele).attr("value");
	}else{
		val = $(ele).val();
	}
	

	//console.log(id, val);

	setOptionToVal(id, val);
	
}

/*
	Sets the value of this option and draw the graphics again
*/
function setOptionToVal(optionID, newVal){
	
	console.log("Setting", optionID, "to", newVal);

	newVal = "" + newVal;
	
	CANCEL_GRAPHICS = true;
	callWasmFunction("setOption", [optionID, newVal], function(val){
	//cjCall("peachtree.options.OptionsAPI", "setOption", optionID, newVal).then(function(val){
		
		//console.log("done", val);
		CANCEL_GRAPHICS = false;
		renderGraphics();
			
	});
	
}


/*
	Opens or closes a tab
*/
function toggleTab(ele){


	// Update css
	$(ele).siblings("li").removeClass("active");
	$(".optionsSlideIn").hide(0);
	
	var hiding = $(ele).hasClass("active");
	$(ele).toggleClass("active");

	var slideID = $(ele).attr("slide");
	var slideEle = $("#" + slideID);
	

	
	if (hiding){
		//console.log("hiding", slideID);
		//slideEle.hide(100);
	}else{
		//console.log("showing", slideID);
		slideEle.show(100);
	}
	


}


/*
	Build a tree from the alignment using the selected method
*/
function buildTree(){
	
	if (BUILDING_TREE) return;
	BUILDING_TREE = true;
	
	let btnID = "#buildTreeBtn";
	
	$(btnID).addClass("disabled");
	addLoader($("#ctrl_loading_div"));
	$(btnID).parent().find(".usermsg").html("");
	
	// Asynchronous call to allow dom to update
	setTimeout(function() {
		callWasmFunction("buildTree", [], function(results){

			removeLoader($("#ctrl_loading_div"));
			$(btnID).removeClass("disabled");
			$(btnID).parent().find(".usermsg").html("Tree built in " + results.time + "ms!").delay(5000).fadeOut();
			BUILDING_TREE = false;
			
			renderGraphics();
			
			
		});
	}, 10);
	
}


/*
	Build a tree from the alignment using the selected method
*/
function reorderTree(){
	
	if (BUILDING_TREE) return;
	BUILDING_TREE = true;
	
	let btnID = "#reorderTreeBtn";
	
	$(btnID).addClass("disabled");
	addLoader($("#ctrl_loading_div"));
	$(btnID).parent().find(".usermsg").html("");
	
	// Asynchronous call to allow dom to update
	setTimeout(function() {
		callWasmFunction("reorderTree", [], function(results){

			removeLoader($("#ctrl_loading_div"));
			$(btnID).removeClass("disabled");
			BUILDING_TREE = false;
			
			renderGraphics();
			
			
		});
	}, 10);
	
}








/*
	Enable/disable the render button based on whether data is available
*/
function updateRenderBtn(){
	
	
	isReadyToRender(function(ready){
		
		if (!ready) {
			$("#renderBtn").addClass("disabled");
		}else{
			$("#renderBtn").removeClass("disabled");
		}
		
		
		
	})
	
}


/*
	Is the data ready to render?
*/
function isReadyToRender(callback = function(response) { }){
	
	callWasmFunction("isReady", [], function(val){
	//cjCall("peachtree.options.OptionsAPI", "isReady").then(function(val){
		
		//val = JSON.parse(val);
		var ready = val.ready;
		//var val = cjStringJavaToJs(val);
		
		callback(ready);
	});
	
}



/*
	Select/deselect a taxon
*/
function toggleTaxon(ele){

	// Update class
	
	ele.toggleClass("selected");
	if (ele.hasClass("selected")) {
		$("#svg .selected").removeClass("main");
		ele.addClass("main");
	}
	
	
	updateSelectionCSS();
	
	// Inform the model
	let index = parseFloat(ele.attr("i"));
	console.log("selecting", index);
	callWasmFunction("selectTaxon", [index]);
	//cjCall("peachtree.aln.AlignmentAPI", "selectTaxon", index);
	
}


/*
	Select everything between the previously selected taxon and the currently selected one 
*/
function selectUpToTaxon(ele){


	
	// Inform the model
	let index = parseFloat(ele.attr("i"));
	callWasmFunction("selectUpToTaxon", [index], function(objects) {
		//cjCall("peachtree.aln.AlignmentAPI", "selectUpToTaxon", index).then(function(val){

		//var objects = JSON.parse(cjStringJavaToJs(val));
		for (let i = 0; i < objects.length; i ++){
			

			var nodeIndex = objects[i].i;
			var nodeSelected = objects[i].select;
			var nodeEle = $("#svg").find("text[i='" + nodeIndex + "']");

			//console.log(objects[i], nodeEle)
			if (nodeSelected){
				$(nodeEle).addClass("selected");
			}else{
				$(nodeEle).removeClass("selected");
			}
			

		}

		updateSelectionCSS();
	});

}

/*
	Flip a subtree
*/
function flipSubtree(ele){
	let index = parseFloat(ele.attr("i"));
	console.log("flipping " + index)

	callWasmFunction("flipSubree", [index], function(val){
	//cjCall("peachtree.phy.PhylogenyAPI", "flipSubree", index).then(function(val){
		renderGraphics();
	});

}




/*
	Clears all taxa selection
*/
function clearSelection(){
	//if ($("#svg .taxon.selected").length == 0) return;
	$("#svg").find(".taxon.selected").removeClass("selected");
	updateSelectionCSS();
	callWasmFunction("clearSelection", [], function(val) {
	//callWasmFunction("clearSelection", [], function(val){
	//cjCall("peachtree.aln.AlignmentAPI", "clearSelection").then(function(val){
		renderGraphics();
	});
	
	
}



/*
	Focuses on taxa
*/
function focusSelection() {
	//if ($("#svg .taxon.selected").length == 0) return;
	console.log("Focusing selection...");
	setOptionToVal("focusOnTaxa", "true");
}

/*
	Focuses on clade
*/
function cladeSelection() {
	//if ($("#svg .taxon.selected").length == 0) return;
	console.log("Focusing clade...");
	setOptionToVal("focusOnClade", "true");
}



/*
	Updates the CSS on selection buttons
*/
function updateSelectionCSS(){
	
	
	$(".selectBtn").removeClass("disabled");
	
	
	/**
	// If nothing is selected, disable buttons
	if ($("#svg .taxon.selected").length == 0){
		$(".selectBtn").addClass("disabled");
	}else{
		$(".selectBtn").removeClass("disabled");
	}
	**/
}


/*
	Populates the taxon search bar auto complete
*/
function populateTaxonSearchBar(){


	callWasmFunction("getListOfTaxaLabels", [], function(results){
	//cjCall("peachtree.aln.AlignmentAPI", "getListOfTaxaLabels").then(function(val){
	//cjCall("peachtree.aln.AlignmentAPI", "getListOfTaxaLabels").then(function(val){
		
		
		//var results = JSON.parse(cjStringJavaToJs(val));
		if (results.err != null){
			console.log(results.err);
		}else{
			$("#taxon_search_input").autocomplete({
				source: results.labels,
				select: searchForTaxon});
				
			// Bind enter keypress
			/*
			$("#taxon_search_input").keydown(function(event){
				if(event.keyCode == 13) {
					//searchForTaxon();
				}
			 });
			 
			 */
		}
		
		
	})
}


/*
	Attempts to find this taxon
*/
function searchForTaxon(){
	
	// Asynchronous
	setTimeout(function() {
		let label = $("#taxon_search_input").val();
		console.log("searchForTaxon", label);
		callWasmFunction("searchForTaxon", [label], function(results){
		//cjCall("peachtree.options.OptionsAPI", "searchForTaxon", label).then(function(){
			renderGraphics();
		});
		
	}, 1);
}










