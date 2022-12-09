$(function() {
    // XXX: Switch to websockets!  Polling status.json is dumb.
    get_settings();
    get_status();
    setInterval(get_status, 5000);
});


// One-time settings on page load
function get_settings() {
    $.getJSON("/theme", options => {
        let selected_theme = options.current;
        let select_theme_obj = $("#select-theme");
        select_theme_obj.empty();
        $.each(options.available, (index, title) => {
            select_theme_obj.append($("<option></option>").attr("value", index).text(title));
        });
        select_theme_obj.val(selected_theme);
    });
}


// Game status every few seconds
function get_status() {
    $.getJSON("/status.json", status => {
        $("#lives").html(status.lives);
        $("#level").html(status.level);
        $("#state").html(status.state);
        $("#brightness").val(status.brightness);
        $("#fw-version").html(status.fw_version);
        $("#fw-track").html(status.fw_track);
        if (status.state == "idle")
            $("#play-button").prop("disabled", "");
        else
            $("#play-button").prop("disabled", "true");
    }).fail((err) => {
        console.log("Could not download /status.json: ", err);
    });
}


function set_brightness() {
    let value = $("#brightness").val();
    $.post("/brightness", value, () => {
        console.log("Setting brightness to:", value);
    });
}


function set_theme() {
    let value = $("#select-theme").val();
    $.post("/theme", value, () => {
        console.log("Setting theme to:", value);
    });
}


function play_button() {
    $.get("/play", response => {
        get_status();
    });
}
