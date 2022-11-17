$(function() {
    // XXX: Switch to websockets!  Polling status.json is dumb.
    setInterval(get_status, 5000);
});


function get_status() {
    $.getJSON("/status.json", status => {
        $("#lives").html(status.lives);
        $("#level").html(status.level);
        $("#state").html(status.state);
        $("#brightness").val(status.brightness);
        $("#firmware").html(status.firmware);
    });
}


function play_button() {
    $.get("/play", response => {
        get_status();
    });
}
