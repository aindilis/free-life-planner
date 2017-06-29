$(document).ready(function() {
  $("body").on('click', '#language-dropdown > li > a', function() {
    $.cookie("appstore-devportal-locale", $(this).attr("data-locale"), {domain: '.amazon.com', expires: 365, path: "/"});
  });
});
