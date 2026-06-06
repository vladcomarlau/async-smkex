
var ws = null;

function setConnected(connected) {
	$("#connect").prop("disabled", connected);
	$("#disconnect").prop("disabled", !connected);
	
	if (connected)
		$("#conversation").show();
	else
		$("#conversation").hide();
		
	$("#messages").html("");
}

function connect() {
	var path = 'ws://localhost:8080/smkex-websocket/' + $("#username").val();
	console.log('Connect to ' + path);

	ws = new WebSocket(path);
	ws.onmessage = function(data) {
		showMessage(data.data);
	}
	setConnected(true);
}

function disconnect() {
	if (ws != null) {
		ws.close();
	}
	setConnected(false);
	console.log("Disconnected");
}

function sendDataViaPostReq() {

	var path = "http://localhost:8080/send-message/" + $("#senderSerial").val() + "/" + $("#recipientSerial").val();
	console.log("POST data to: " + path);

	fetch(path, {
		method: "POST",
		body: $("#message").val()
	}).then(res => {
		console.log("Request complete! response:", res);
	});
}

function showMessage(message) {
	$("#messages").append("<tr><td> " + message + "</td></tr>");
}

$(document).ready(function() {
	console.log("Index page is ready");
	
	$("#connect").click(function() { connect(); });
	$("#disconnect").click(function() { disconnect(); });
	$("#send").click(function() { sendDataViaPostReq(); });
});

