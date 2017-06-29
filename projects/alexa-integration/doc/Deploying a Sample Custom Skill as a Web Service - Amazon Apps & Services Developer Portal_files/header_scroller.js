var win = $(window);
var lower_header = document.getElementById("lower-header");
win.scroll(function() {
	//hide or show upper header base on the scroll position
	if(win.scrollTop() > 32) {
		lower_header.style.position = "fixed";
		lower_header.style.top = "0px";
		lower_header.className = "with-shadow";
	}
	else {
		lower_header.style.position = "absolute";
		lower_header.style.top = "32px";
		lower_header.className = "";
	}
});
