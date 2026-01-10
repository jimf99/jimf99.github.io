const log = document.getElementById("log");
const ws = new WebSocket(`ws://${location.host}/ws`);

ws.onmessage = e => {
  log.textContent += e.data + "\n";
};

function send() {
  const input = document.getElementById("msg");
  ws.send(input.value);
  input.value = "";
}
