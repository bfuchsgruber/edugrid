function toggleSwitch(element) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/updatevalues?ID=" + element.id + "&STATE=1", true);
    xhr.send();
}

function toggleSlider(element) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/updatevalues?ID=" + element.id + "&STATE=" + element.value, true);
    xhr.send();
}

function labelHit(element) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/updatevalues?ID=" + element.id + "&STATE=" + element.value, true);
    xhr.send();
}


var socket = new WebSocket("ws://192.168.1.1:81/");
socket.onmessage = function (event) {
    var json_doc_str = event.data; // load JSON like String
    var json_doc = JSON.parse(json_doc_str); // parse String into a real JSON

    var pwm = json_doc["pwm"];
    var pwm_label_text = document.getElementById("pwm_label");
    pwm_label_text.innerHTML = pwm;

    var freq = json_doc["freq"];
    var freq_label_text = document.getElementById("freq_label");
    freq_label_text.innerHTML = freq;


    // mode 0 = Auto
    // mode 1 = manually
    var mode = json_doc["mode"];
    var pwm_int = json_doc["pwm_raw"];
    var mode_slider_state = document.getElementById("1") // switch ID
    var mode_label_text = document.getElementById("mode_label"); // label for mpp state
    var slider_range = document.getElementById("4");
    if (mode == "0") {
        mode_label_text.innerHTML = "AUTO";
        mode_slider_state.checked = true;
        slider_range.value = pwm_int;
    } else if (mode == "1") {
        mode_label_text.innerHTML = "MANU";
        mode_slider_state.checked = false;
        slider_range.value = pwm_int;
    }

    var current_in = json_doc["current_in"];
    var current_in_label_text = document.getElementById("current_in_label");
    current_in_label_text.innerHTML = current_in;

    var current_out = json_doc["current_out"];
    var current_out_label_text = document.getElementById("current_out_label");
    current_out_label_text.innerHTML = current_out;

    var voltage_in = json_doc["voltage_in"];
    var voltage_in_label_text = document.getElementById("voltage_in_label");
    voltage_in_label_text.innerHTML = voltage_in;

    var voltage_out = json_doc["voltage_out"];
    var voltage_out_label_text = document.getElementById("voltage_out_label");
    voltage_out_label_text.innerHTML = voltage_out;

    var power_in = json_doc["power_in"];
    var power_in_label_text = document.getElementById("power_in_label");
    power_in_label_text.innerHTML = power_in;

    var power_out = json_doc["power_out"];
    var power_out_label_text = document.getElementById("power_out_label");
    power_out_label_text.innerHTML = power_out;

    var efficiency = json_doc["efficiency"];
    var efficiency_label_text = document.getElementById("efficiency_label");
    efficiency_label_text.innerHTML = String(efficiency);

    var simulation_state = json_doc["simulation"];
    var simulation_label_text = document.getElementById("simulation_label");
    simulation_label_text.innerHTML = simulation_state;

    var logging_state = json_doc["logging"];
    var logging_label_text = document.getElementById("logging_label");
    logging_label_text.innerHTML = logging_state;
}