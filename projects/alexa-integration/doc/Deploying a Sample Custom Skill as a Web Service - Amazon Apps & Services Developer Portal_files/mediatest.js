$(document).ready(function() {
    $.ajax({
        url : '/dsv/dsv_games/application/binary/keep-alive-ping.html',
        type : 'GET',
        dataType : 'json',
        data: { ts: $.now() },
        async: true
    });
});


// toggle search on resize
$(window).resize(function() {
    if (($("#search-box-form").hasClass("toggle")) && $("#search-box-toggle-button").is(":hidden")) {
    $("#search-box-form").removeClass("toggle");
    $("#search-box-submit").show();
    }
});
