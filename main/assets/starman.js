$(function() {
    console.log( "ready!" );
});

function play_button() {
    $.get("/play", function(result) {
        console.log(result);
    });
}