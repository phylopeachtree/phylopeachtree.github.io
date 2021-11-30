


function initUtil(){
	

	STOP_LOADING = false;

	$("#control_panel_div").hide(0);
	$("#graphics_div").hide(0);
	$("#upload_div").show(0);
	BUILDING_TREE = false;

	// Is shift key down right now
	SHIFT_KEY_IS_DOWN = false;
	CTRL_KEY_IS_DOWN = false;

	window.onkeyup = function(e) { 
		if (SHIFT_KEY_IS_DOWN && e.keyCode == 16) {
			//console.log("shift up");
			SHIFT_KEY_IS_DOWN = false;
		}
		
		if (CTRL_KEY_IS_DOWN && e.ctrlKey) {
			//console.log("ctrl up");
			CTRL_KEY_IS_DOWN = false;
		}
		
	}
	window.onkeydown = function(e) { 
		if (!SHIFT_KEY_IS_DOWN && e.keyCode == 16) {
			//console.log("shift down");
			SHIFT_KEY_IS_DOWN = true;
		}
		
		
		if (!CTRL_KEY_IS_DOWN && e.ctrlKey){
			CTRL_KEY_IS_DOWN = true;
		}

		if (!SHIFT_KEY_IS_DOWN && !$("#taxon_search_input").is(":focus")){
			
			// Left
			if (e.keyCode == 37) {
				scrollHorizontal(false, CTRL_KEY_IS_DOWN);
			}
			
			// Right
			else if (e.keyCode == 39) {
				scrollHorizontal(true, CTRL_KEY_IS_DOWN);
			}
			
			// Up
			else if (e.keyCode == 38) {
				scrollVertical(true, CTRL_KEY_IS_DOWN);
			}
			
			// Down
			else if (e.keyCode == 40) {
				scrollVertical(false, CTRL_KEY_IS_DOWN);
			}
		
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
			$("#aln_upload .usermsg").hide(0);
			
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
			$("#phy_upload .usermsg").hide(0);
			
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
			$("#epi_upload .usermsg").hide(0);

			
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
	$(uploadSelector + " .usermsg").show(0);
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

html += "<span selector='" + uploadSelector + "'' onclick='removeUpload(this)' class='removeUploadBtn' title='Remove uploaded file'> (&#9249;)</span>"

	$(uploadSelector + " .usermsg").html(html);
	$(uploadSelector + " .usermsg").show(0);

	
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
		$(id + " .usermsg").hide(0);
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
 * Cause the navigation peach to start spinning, and the cursor to start loading
 */
function startLoading(){
	
	
	// Only add loading if it takes a while
	//setTimeout(function() {
		//if (STOP_LOADING) return;
		//STOP_LOADING=false;
		//if (!$("#peachWheel").hasClass("loading")) $("#peachWheel").addClass("loading");
		if (!$("body").hasClass("loading")) $("body").addClass("loading");
	//}, 50);
}

function stopLoading(){
	//STOP_LOADING=true;
	//$("#peachWheel").removeClass("loading");
	$("body").removeClass("loading");
}


/*
	Adds a loading icon
*/
function addLoader(ele, large=false){
	
	
	if (ele.find(".loader").length > 0) return;
	
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
	ele.find(".loader").remove();
	$("body").removeClass("loading");
}


/*
	Display all user options
*/
function renderOptions(){
	
	if (BUILDING_TREE) return;
	
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
						
						
						if (opt.domain != null) {
							
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
			
			
			// Tree ready to show?
			if (result.ready) {
				$("#btn_clade").show(100);
				$("#downloadTreeBtn").show(100);
			}
			else {
				$("#btn_clade").hide(0);
				$("#downloadTreeBtn").hide(0);
			}
			
			
			// Is there a real alignment there?
			callWasmFunction("alignmentIsMock", [], function(result2){
			
				if (!result2.mock) {
					$("#downloadAlignmentBtn").show(0);
				}
				else {
					$("#downloadAlignmentBtn").hide(0);
				}
				
				if (result.ready || result2.mock){
					$("#buildTreeBtn").hide(0);
				}else{
					$("#buildTreeBtn").show(100);
				}
			
			
				
			
			
			
			});
			
			

			
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
function setOptionToVal(optionID, newVal, resolve = function() { }){
	
	console.log("Setting", optionID, "to", newVal);

	newVal = "" + newVal;
	
	CANCEL_GRAPHICS = true;
	callWasmFunction("setOption", [optionID, newVal], function(val){
	//cjCall("peachtree.options.OptionsAPI", "setOption", optionID, newVal).then(function(val){
		
		//console.log("done", val);
		CANCEL_GRAPHICS = false;
		renderGraphics();
		resolve();
			
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
	CANCEL_TREE_BUILDING = false;
	
	let btnID = "#buildTreeBtn";
	
	$(btnID).addClass("disabled");
	

	
	// Open a dialog
	var html = `<div id="treeBuildingDialog">
					<div>
						<h3>Building Neighbour Joining Tree</h3>
						This may take some time...
						
						<br><br>
						
						<span id="cancelBuildBtn" onclick="cancelTreeBuilding();" class="button" title="Cancel tree building">Cancel</span>
						
						<br><br>

						<div id="treeBuildingProgressBar" class="progressBar">
							<div> </div>
						</div>
						<div id="treeBuildingTimer">
							Initialising site patterns...
						</div>
						
						
						
					</div>
				</div>`;
	$("body").append(html);
	$("body").addClass("loading");
	
	$("#main").addClass("faded");

	
	
	
	
	// Asynchronous call to allow dom to update
	setTimeout(function() {
		callWasmFunction("buildTree", [], function(results){
			
			resumeTreeBuilding(btnID);
			
			
			
		});
		
	}, 10);
	
}




/*
	Cancels tree building
*/
function cancelTreeBuilding(){
	if (BUILDING_TREE) CANCEL_TREE_BUILDING = true;
}

/*
	Resumes tree building
*/
function resumeTreeBuilding(btnID){
	
	

	// Loading
	callWasmFunction("resumeTreeBuilding", [(CANCEL_TREE_BUILDING ? 1 : 0)], function(results){
		

		var finished = results.finished;
		
	
		// Update progress
		var progress = results.progress;
		var time = Math.round(results.time / 100) / 10;
		if (progress > 1) progress = 1;
		
		
		// Loading meter
		var fullWidth = parseFloat($("#treeBuildingProgressBar").width());
		var loaderWidth = progress*fullWidth;
		$("#treeBuildingProgressBar").find("div").width(loaderWidth);
		
		
		// Progress update
		var progressPercentage = Math.round(progress * 1000) / 10;
		$("#treeBuildingTimer").html(progressPercentage + "% (" + time + "s elapsed)");
		
		//console.log("progress", progress, loaderWidth, fullWidth);
		
		
		
		

		if (finished){
			
			console.log("done!");


			$(btnID).removeClass("disabled");

			
			$("#treeBuildingDialog").remove();
			$("body").removeClass("loading");
			$("#main").removeClass("faded");
			
			
			BUILDING_TREE = false;
			CANCEL_TREE_BUILDING = false;
			
			renderGraphics();
			
		}else{
			

			resumeTreeBuilding(btnID);
			
		}
		

		
	});
	
	
}


/*
	Build a tree from the alignment using the selected method
*/
function reorderTree(){
	
	if (BUILDING_TREE) return;
	BUILDING_TREE = true;
	
	let btnID = "#reorderTreeBtn";
	
	$(btnID).addClass("disabled");
	//addLoader($("#ctrl_loading_div"));
	startLoading();
	
	// Asynchronous call to allow dom to update
	setTimeout(function() {
		callWasmFunction("reorderTree", [], function(results){

			//removeLoader($("#ctrl_loading_div"));
			stopLoading();
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
	//addLoader($("#ctrl_loading_div"));
	startLoading();
	callWasmFunction("clearSelection", [], function(val) {
		renderGraphics(function(){
			//removeLoader($("#ctrl_loading_div"));
			stopLoading();
		});
		
	});
	
	
}



/*
	Focuses on taxa
*/
function focusSelection() {
	//if ($("#svg .taxon.selected").length == 0) return;
	console.log("Focusing selection...");
	//addLoader($("#ctrl_loading_div"));
	startLoading();
	setOptionToVal("focusOnTaxa", "true", function() { 
		//removeLoader($("#ctrl_loading_div"));
		stopLoading();
	});
}

/*
	Focuses on clade
*/
function cladeSelection() {
	//if ($("#svg .taxon.selected").length == 0) return;
	console.log("Focusing clade...");
	//addLoader($("#ctrl_loading_div"));
	startLoading();
	setOptionToVal("focusOnClade", "true", function() { 
		//removeLoader($("#ctrl_loading_div")); 
		stopLoading();
	});
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







/*
	Redirect to documentation page
*/
function goToDocumentation(){
	window.location.href = '/about';
}

