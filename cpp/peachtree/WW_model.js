
self.importScripts('peachtree.js');
WASM_MESSAGE_LISTENER = {};


// Send a message back to the original JS module to inform that the program has initialised
onRuntimeInitialised = function(){
    //Module.ccall("initGUI", null, [], []); // Initialise the WASM module for GUI use
    postMessage("wasm_initialised");
}
Module['onRuntimeInitialized'] = onRuntimeInitialised;


// Receive a message from the webassembly module
messageFromWasmToJS = function(msg = null, msgID = null){

    //console.log("message", msg, msgID);


    // See if the message has a callback
    if (msgID != null && WASM_MESSAGE_LISTENER[msgID] != null){


        if (msg == null || msg == ""){
            WASM_MESSAGE_LISTENER[msgID].resolve("done");
        }else{

            //console.log("msg", msg);

            // Convert the message into a string that can be parsed directly by JSON
            msg = msg.replace(/'/g, '\"').replace(/'/g, "\\'").replace(/"\[/g, "[").replace(/\]"/g, "]");

            //console.log("msg", msgID, msg);

            WASM_MESSAGE_LISTENER[msgID].resolve(msg)

        }

        // Do not delete the message listener event if requested 
        if (WASM_MESSAGE_LISTENER[msgID] != null && (WASM_MESSAGE_LISTENER[msgID].remove == null || WASM_MESSAGE_LISTENER[msgID].remove == true)){
            WASM_MESSAGE_LISTENER[msgID] = null;
        }
    }

}


// Converts a javascript object into three parts: 
//      - A string containing all the dict names (separated by ,)
//      - A buffer containing the values. Can be double, int or string
//      - A number containing the length of the array
getCppArrayFromDict = function(dict, dataType = "double"){


    // Convert dict into js list
    var dictKeyString = "";
    var arrayDataToParse = [];
    for (key in dict){
        dictKeyString += key + ",";
        arrayDataToParse.push(dict[key]);
    }
    dictKeyString = dictKeyString.substring(0, dictKeyString.length-1); // Remove the final ,



    // Convert the js list into a typed list
    var typedArray = dataType == "double" ? new Float64Array(arrayDataToParse.length) : dataType == "int" ? new Int32Array(arrayDataToParse.length) : "";
    for (var i = 0; i < arrayDataToParse.length; i++) {
        if (dataType != "string") typedArray[i] = arrayDataToParse[i];
        else {
            typedArray += arrayDataToParse[i];
            if (i < arrayDataToParse.length-1) typedArray += ",";
        }
    }


    // Assign data to the heap
    var buffer;// = Module._malloc(typedArray.length * typedArray.BYTES_PER_ELEMENT)
    if (dataType == "double") Module.HEAPF64.set(typedArray, buffer >> 2);
    else if(dataType == "int") Module.HEAP32.set(typedArray, buffer >> 2);
    else if(dataType == "string") buffer = typedArray;

    // Return object
    return {keys: dictKeyString, vals: buffer, len: arrayDataToParse.length};


}





onmessage = function(e) {

    var json = JSON.parse(e.data)

    //console.log(json)

    var id = json.msgID;
    var fn = json.fn;
    var params = json.params;
    var useHeap = json.useHeap;


    // Module.ccall("test", null, [], []);
    //var fnName = strsplit(fn)

    //eval(fn)();

    // Parameter types. 
    var types = [];
    for (var param in params){
        var p = params[param];
        if (typeof p  === 'string') types.push("string");
        else types.push("number");
    }



    //console.log(fn, params, types);



    // Large data?
    if (useHeap){

        //console.log("A lot of data. Using malloc")

        //var bufs = [];
        //var isBuf = [];
        for (var param in params){

            var p = params[param];
            if (typeof p  === 'string'){

                // https://github.com/emscripten-core/emscripten/issues/6860
                console.log(param, "is a large string", p.length);

                const bufferSize = lengthBytesUTF8(p);
                const bufferPtr = Module._malloc(bufferSize + 1);
                stringToUTF8(p, bufferPtr, bufferSize + 1);

                const putOnHeap = Module.cwrap('putOnHeap', null, ['number']); // not 'string', pointer is a number
                putOnHeap(bufferPtr);
                Module._free(bufferPtr);

                params.splice(param, 1);
                types.splice(param, 1);

                break;

            }
               // var buf = Module._malloc(p.length*2); // 2 bytes per element
                //Module.HEAPU8.set(p, buf);
                //bufs.push(buf);
                //isBuf.push(true);
                //types[param] = "number"; // Pointer is a number
           // }else{
                //bufs.push(p);
                //isBuf.push(false);
           // }

             

        }

       
        /*
        console.log(types, bufs)
       
        Module.ccall(fn, null, types, bufs);
        for (var i = 0; i < buf.length; i++){
            if (isBuf[i]){
                console.log("freeing", buf);
                var buf = bufs[i];
                Module._free(buf);
            }
        }
        */

    }

    // Msg id
    Module.ccall("setCurrentMsgID", null, ["number"], [id]);


    // Resolve
    var resolve = function(resultStr) {
        var json = {msgID: id, result: resultStr}
        postMessage(JSON.stringify(json));
    }
    WASM_MESSAGE_LISTENER[id] = {resolve: resolve};

    //console.log("Small data")
    //console.log(types, params)
    Module.ccall(fn, null, types, params); 


};



