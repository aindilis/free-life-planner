//modify the style of text banner if summary is too long
var text_banner = $("#text-banner");
if(text_banner.prop("scrollHeight") > 60) {
	text_banner.css("border-bottom", "none");
	var p = $("#text-banner > p");
	//p.css("border-top", "2px solid #f9991d");
	p.css("width", "99%");
}