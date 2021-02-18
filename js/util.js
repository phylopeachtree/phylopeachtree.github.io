


function initUtil(){
	
	
	$("#graphics_div").hide(0);
	$("#upload_div").show(0);
	
	

	// Alignment uploader
	var speciesTreeDropzone = new Dropzone("#aln_upload", { url: "/file/post"});
	speciesTreeDropzone.on("addedfile", function(file) {
		
		
		var reader = new FileReader();


	

		// Closure to capture the file information.
		reader.onload = (function(theFile) {
			
			
			
			return function(e) {
				
				try {
				
					
					//parseSpeciesTree(e, util_file);
					if (e == null || e.target == null || e.target.result == null){
						throw {message: "Cannot open file"};
					}
					var contents = e.target.result;
					cjCall("peatree.aln.AlignmentAPI", "uploadAlignment", contents).then(function(val){
						
						try {
							var val = JSON.parse(cjStringJavaToJs(val));
							console.log(val);
							if (val.err != null) {
								throw {message: val.err};
							}
						}catch(err){
							 return plotUploadErrorMsg(err, "#aln_upload");
							 
						}
						return plotUploadSuccessMsg(file.name, "#aln_upload");
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
	return false;
}


/*
	Clear error message on success
*/
function plotUploadSuccessMsg(filename, uploadSelector){
	$(uploadSelector + " .usermsg").html(filename + " successfully uploaded!");
	updateRenderBtn();
	return true;
}


/*
	Display all user options
*/
function renderOptions(){
	
	cjCall("peatree.options.OptionsAPI", "getOptions").then(function(val){
						
			
		var val = JSON.parse(cjStringJavaToJs(val));
		console.log("options", val);
		
		$("#options_div").html(JSON.stringify(val));
				
	});
	
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
	
	cjCall("peatree.options.OptionsAPI", "isReady").then(function(val){
		val =  val.value0 == 1;
		console.log("ready", val);
		//var val = cjStringJavaToJs(val);
		
		callback(val);
	});
	
}






