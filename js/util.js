


function initUtil(){
	
	
	$("#graphics_div").hide(0);
	$("#upload_div").show(0);
	BUILDING_TREE = false;
	
	

	// Alignment uploader
	var speciesTreeDropzone = new Dropzone("#aln_upload", { url: "/file/post"});
	speciesTreeDropzone.on("addedfile", function(file) {
		
		
		var reader = new FileReader();


	

		// Closure to capture the file information.
		reader.onload = (function(theFile) {
			
			addLoader($("#aln_upload_title"));
			
			return function(e) {
				
				try {
				
					
					//parseSpeciesTree(e, util_file);
					if (e == null || e.target == null || e.target.result == null){
						throw {message: "Cannot open file"};
					}
					var contents = e.target.result;
					cjCall("peachtree.aln.AlignmentAPI", "uploadAlignment", contents).then(function(val){
						
						try {
							var val = JSON.parse(cjStringJavaToJs(val));
							console.log(val);
							if (val.err != null) {
								throw {message: val.err};
							}
						}catch(err){
							 return plotUploadErrorMsg(err, "#aln_upload");
							 
						}
						
				
						return plotUploadSuccessMsg(file.name, val.time, "#aln_upload");
					});
					
					
				}catch(err){
					return plotUploadErrorMsg(err, "#aln_upload");
				}
				
			
				
				
			};

		})(file);

		reader.readAsText(file);
		
		
		

	});
	
	
	
	
	
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
	if (time == "0" || time == 0) time = "<1";
	$(uploadSelector + " .usermsg").html(filename + " successfully parsed in " + time + "ms!");
	updateRenderBtn();
	removeLoader($(uploadSelector + "_title"));
	return true;
}


/*
	Adds a loading icon
*/
function addLoader(ele, large=false){
	if (large){
		ele.append(`<div title="Loading..." class="loader large"></div>`);
	}else{
		ele.append(`<div title="Loading..." class="loader"></div>`);
	}
	
}

function removeLoader(ele){
	ele.find(" .loader").remove();
}


/*
	Display all user options
*/
function renderOptions(){
	
	cjCall("peachtree.options.OptionsAPI", "getOptions").then(function(val){
						
			
		var options = JSON.parse(cjStringJavaToJs(val));
		console.log("options", options);


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
			if (opt.isBoundary) continue;
			let s = opt.section;
			if (!sections.includes(s)) sections.push(s);

		}
		console.log("sections", sections);

		// Create one tab per section
		for (let i = 0; i < sections.length; i++){


			var slideInID = "options_" + sections[i];

			// Tab
			optionsTabs.append("<li onClick='toggleTab(this)' title='Open/close tab' slide='" + slideInID + "'>" + sections[i] + "</li>")


			// Get the options of this section
			let optionsHTML = "";
			for (let o = 0; o < options.length; o ++){
				let opt = options[o];
				if (opt.isBoundary) continue;
				if (opt.section == sections[i]){
					
					
					if (opt.type == "NumericalOption"){
						
						// Options html
						optionsHTML += `
						<div class="optionsBox">
							
							<div class="slidecontainer">
								 <input onChange="setOptionFromEle(this)" type="range" min="` + opt.min + `" max="` + opt.max + `" value="` + opt.value + `" step="` + opt.step + `" class="slider" var="` + opt.name + `">
							</div>
							` + opt.title + ` (` + opt.value + `)
						</div>`;
						
					}
					
					

					
					
					
				}
			}
			
			
			


			// Slide in html
			var slideHTML = `<div class="optionsSlideIn" style="display:none">
				<div>
					<h2>
					` + sections[i] + ` settings
					</h2>
					<div>
					` + optionsHTML + `
					</div>
				</div>
			</div>`;
			var div = $(slideHTML)
			div.attr("id", slideInID);
			slideIns.append(div)

		}

		
		//.html(JSON.stringify(options));
				
	});
	
}


/*
	Sets the value of this option from the element
*/
function setOptionFromEle(ele){
	
	let id = $(ele).attr("var");
	let val = $(ele).val();
	setOptionToVal(id, val);
	
}

/*
	Sets the value of this option and draw the graphics again
*/
function setOptionToVal(optionID, newVal){
	
	console.log("Setting", optionID, "to", newVal);
	
	CANCEL_GRAPHICS = true;
	cjCall("peachtree.options.OptionsAPI", "setOption", optionID, newVal).then(function(val){
		
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
	addLoader($(btnID).parent());
	$(btnID).parent().find(".usermsg").html("");
	
	// Asynchronous call to allow dom to update
	setTimeout(function() {
		cjCall("peachtree.options.OptionsAPI", "buildTree").then(function(results){
			
			var results = JSON.parse(cjStringJavaToJs(results));
			
			console.log("tree", results.newick);
			removeLoader($(btnID).parent());
			$(btnID).removeClass("disabled");
			$(btnID).parent().find(".usermsg").html("Tree built in " + results.time + "ms!");
			BUILDING_TREE = false;
			
			renderGraphics();
			
			
		});
	}, 1);
	
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
	
	cjCall("peachtree.options.OptionsAPI", "isReady").then(function(val){
		val =  val.value0 == 1;
		console.log("ready", val);
		//var val = cjStringJavaToJs(val);
		
		callback(val);
	});
	
}









