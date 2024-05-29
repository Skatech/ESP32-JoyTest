var ws;
var wsm_max_len = 4096; /* bigger length causes uart0 buffer overflow with low speed smart device */

const MODE_DIRECT = 0, MODE_INDIRECT = 1
const state = { mode: MODE_DIRECT, x: 0, y: 0, joyx: 0, joyy: 0, joyb: 0 }

function updateCanvas() {
  const cnv = /** @type {HTMLCanvasElement} */ (document.getElementById("canvas"))
  const width = cnv.width = cnv.parentElement.clientWidth
  const height = cnv.height = cnv.parentElement.clientHeight
  
  const ctx = cnv.getContext("2d")
  ctx.save()
  ctx.translate(0.5, 0.5)

  if (state.joyb == 10) {
    if (state.mode == MODE_DIRECT) {
      state.mode = MODE_INDIRECT
      cnv.style.background = "#fff0f5"
      state.x = width / 2
      state.y = height / 2
      console.log('switched to indirect mode')
    }
    else {
      state.mode = MODE_DIRECT
      cnv.style.background = "#f0f5ff"
      console.log('switched to direct mode')
    }
  }

  if (state.mode == MODE_DIRECT) {
    state.x = width / 2 + width * state.joyx / 2
    state.y = height / 2 - height * state.joyy / 2   
  }
  else {
    state.x = Math.max(0, Math.min(width, state.x + width / 200 * state.joyx))
    state.y = Math.max(0, Math.min(height, state.y - height / 200 * state.joyy))
  }

  ctx.clearRect(0, 0, cnv.width, cnv.height)
  ctx.lineWidth = 1
  ctx.strokeStyle = "lightgray"
  ctx.beginPath()
  ctx.moveTo(0, height / 2)
  ctx.lineTo(width, height / 2)
  ctx.moveTo(width / 2, 0)
  ctx.lineTo(width / 2, height)
  ctx.closePath()
  ctx.stroke()

  ctx.strokeStyle = ctx.fillStyle = "red"
  ctx.beginPath()
  ctx.moveTo(0, state.y, 0)
  ctx.lineTo(width, state.y)
  ctx.moveTo(state.x, 0)
  ctx.lineTo(state.x, height)
  ctx.closePath()
  ctx.stroke()

  ctx.arc(state.x, state.y, 10, 0, Math.PI * 2, false)
  ctx.fill()
  ctx.restore()
}

function update_text(text) {
  var chat_messages = document.getElementById("messages");
  chat_messages.innerHTML += text + '<br>';
  chat_messages.scrollTop = chat_messages.scrollHeight;
}

function send_onclick() {
  if(ws != null) {
    var message = document.getElementById("message").value;
    
    if (message) {
      document.getElementById("message").value = "";
      ws.send(message + "\n");
      update_text('<span style="color:navy">' + message + '</span>');
      // You can send the message to the server or process it as needed
    }
  }
}

function connect_onclick() {
  if (window.location.host.length < 1) {
    console.log("Cant connect to ws, reason - no host")
  }
  else {
    if (ws == null) {
      ws = new WebSocket("ws://" + window.location.host + ":81");
      // ws = new WebSocket(`ws://${window.location.hostname}/ws`); // async WS
      ws.onopen = ws_onopen;
      ws.onclose = ws_onclose;
      ws.onmessage = ws_onmessage;
      document.getElementById("ws_state").innerHTML = "CONNECTING";
    } else
      ws.close();
  }
}

function ws_onopen() {
  document.getElementById("ws_state").innerHTML = "<span style='color:blue'>CONNECTED</span>";
  document.getElementById("bt_connect").innerHTML = "Disconnect";
  document.getElementById("messages").innerHTML = "";
}

function ws_onclose() {
  document.getElementById("ws_state").innerHTML = "<span style='color:gray'>CLOSED</span>";
  document.getElementById("bt_connect").innerHTML = "Connect";
  ws.onopen = null;
  ws.onclose = null;
  ws.onmessage = null;
  ws = null;
}

function ws_onmessage(e_msg) {
  const joystickPrefix = ">joy:"
  if (e_msg.data.startsWith(joystickPrefix)) {
    const values = e_msg.data.substring(joystickPrefix.length).split(",")
    const x = parseInt(values[0])
    const y = parseInt(values[1])
    const b = parseInt(values[2])
    document.getElementById("temperature-value").innerText = `${x}, ${y}, ${b}`

    state.joyx = (x - 512) / 512
    state.joyy = (y - 512) / 512
    state.joyb = b
    updateCanvas()
  }
  else update_text('<span style="color:blue">' + e_msg.data + '</span>');
}

function upd(x, y, b = false) {
  ws_onmessage({data:">joy:" + x + ',' + y + ',' + b}) 
}

