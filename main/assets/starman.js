$(function() {
    // XXX: Switch to websockets!  Polling status.json is dumb.
    get_status();
    setInterval(get_status, 5000);
});


function get_status() {
    $.getJSON("/status.json", status => {
        $("#lives").html(status.lives);
        $("#level").html(status.level);
        $("#state").html(status.state);
        $("#brightness").val(status.brightness);
        $("#firmware").html(status.firmware);
        if (status.state == "idle")
            $("#play-button").prop("disabled", "");
        else
            $("#play-button").prop("disabled", "true");
    });
}


function set_brightness() {
    let value = $("#brightness").val();
    $.post("/brightness", value, () => {
        console.log("Setting brightness to:", value);
    });
}


function play_button() {
    $.get("/play", response => {
        get_status();
    });
}
