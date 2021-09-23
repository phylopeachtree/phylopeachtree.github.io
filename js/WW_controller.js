





MESSAGE_LISTENER = {};

function register_WebWorker(resolve = function() { }, dir = "cpp/peachtree/WW_model.js"){


	WEB_WORKER_WASM = null;
	if(typeof(Worker) !== "undefined") {
		try {



			 	// Setup webworker for webassembly
			    WEB_WORKER_WASM = new Worker(dir);




    			// Tell the WebWorker to initialise
    			
					//callWebWorkerFunction(function() { WW_JS.init_WW(true); });


		} catch(err){
			// WebWorker failed to register but we must continue to use the variables/functions from the WebWorker.js file anyway
			WEB_WORKER_WASM = null;
			console.log('WebWorker registration failed', err);
		}


    } else {
       	console.log('WebWorker registration failed');
    }


    // Set up message listeners
    MESSAGE_LISTENER = {};

	//$("#output_asm").show(true);

	// Connect to webassembly
	fetch("cpp/peachtree/peachtree.wasm").then(response =>
		 response.arrayBuffer()
	).then(bytes =>
		WebAssembly.compile(bytes)
	);


		 // Error handler
    WEB_WORKER_WASM.onerror = function (err) {
    	console.log("WASM error", err);
	}

	WEB_WORKER_WASM.onmessage = function(event) {
		onWebWorkerReceiveMessage(event, resolve);
	}



}


function onWebWorkerReceiveMessage(event, resolveAfterWebassemblyInitialised = function() { }){

	//console.log("Received message", event.data);


	if (event.data == "wasm_initialised"){
		console.log("wasm_initialised");
		resolveAfterWebassemblyInitialised();
		return;
	}



	var json = JSON.parse(event.data);


	// Otherwise the message will be the following format "id~X~result"
	var id = json.msgID;
	var result = json.result;


	// Find the object in MESSAGE_LISTENER with the corresponding id
	var obj = MESSAGE_LISTENER[id];
	if (obj == null) return;


	// If the result is 'done' then we can continue with the callback
	if (result == "done"){
		obj["resolve"](null);
	}


	else{
		// Otherwise we continue with the callback but with the JSON-parsed parameter
		//console.log("result", id, result);
		try {
			var resultTemp = JSON.parse(result);
			result = resultTemp;
		}catch(e){
			console.log("Could not parse", result);
			JSON.parse(result);
			return;
		}

		//console.log("Returning JSON ", result);
		obj["resolve"](result);
	}


	// Remove this object from the MESSAGE_LISTENER list
	if (MESSAGE_LISTENER[id] != null && (MESSAGE_LISTENER[id].remove == null || MESSAGE_LISTENER[id].remove == true)) MESSAGE_LISTENER[id] = null;
}



function callWebWorkerFunction(fn, resolve = null, msgID = null, removeAfter = true){


	

	var fnStr = fn; // "(" + fn + ")";

	console.log("Calling ", fnStr, msgID);

	//console.log("Posting msg " + fnStr);

	// If we do not require a response then just send the message
	if (msgID == null) WEB_WORKER_WASM.postMessage("~X~" + fnStr);

	// Otherwise add this to MESSAGE_LISTENER with a unique id and send the message along with the id
	else{
		MESSAGE_LISTENER[msgID] = {resolve: resolve, remove: removeAfter};
		WEB_WORKER_WASM.postMessage(msgID + "~X~" + fnStr);
	}



}



// Assumes that function is in format fn_name(arg1 | arg2)
function executeFunctionFromString(fnStr){

	if (fnStr == "null" || fnStr == "false") return;


	fnStr = fnStr.trim();
	fnStr = fnStr.substring(0, fnStr.length - 1); // Remove the last ')'
	var bits = fnStr.split("(");
	var fnName = bits[0]
	var args = [];
	var argStr = bits[1].trim().split("|");

	for (var argNum = 0; argNum < argStr.length; argNum ++){
		if (argStr[argNum].trim() == "") continue;
		args.push(JSON.parse(argStr[argNum]));
	}


	if (args.length == 0) window[fnName]();
	else if (args.length == 1) window[fnName](args[0]);
	else if (args.length == 2) window[fnName](args[0], args[1]);
	else if (args.length == 3) window[fnName](args[0], args[1], args[2]);

}



function callWasmFunction(fnName, params=[], resolve = function() { }, useHeap=false){




	// Create and a message id
	var msgID = Math.ceil(Math.random() * 1000);
	while (MESSAGE_LISTENER["" + msgID] != null) msgID ++;
	MESSAGE_LISTENER[msgID] = {resolve: resolve};

	var json = {msgID: msgID, fn : fnName, params: params, useHeap: useHeap}

	WEB_WORKER_WASM.postMessage(JSON.stringify(json));




}



