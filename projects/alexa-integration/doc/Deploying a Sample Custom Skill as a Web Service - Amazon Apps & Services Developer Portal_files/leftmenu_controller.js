//slide in/out the third level menu on click
toggleThirdLevelMenu = function() {
	if(third_level_menu_arrow.hasClass("triangle-left")) {
		third_level_menu.css("left", "15px");
		third_level_menu.css("box-shadow", "-2px 0px 3px #bdc3c7");
		third_level_menu_arrow.removeClass("triangle-left");
		third_level_menu_arrow.addClass("triangle-right");
	}
	else {
		third_level_menu.css("left", "100%");
		third_level_menu.css("box-shadow", "none");
		third_level_menu_arrow.removeClass("triangle-right");
		third_level_menu_arrow.addClass("triangle-left");
	}
};

//set the style of third level menu in the left menu
third_level_menu = $("#left-sidebar-list > li > ul > li > ul");
if(third_level_menu.length) {
	var third_level_parent = third_level_menu.parent();
	if(third_level_parent.hasClass("hidden")) {
		var hidden_ul = third_level_parent.parent();
		hidden_ul.css("height", "0px");
		hidden_ul.parent().addClass("grand-expanded");
		
	}
	else {
		third_level_parent.addClass("expanded");
		third_level_parent.removeClass("active");
	}
	third_level_menu.addClass("lockheight");
	var max_height = Math.max(third_level_menu.prop("scrollHeight"), $("#left-sidebar").height(), $("#content").height());
	$("#left-sidebar").height(max_height);
	third_level_menu.height(max_height);
	third_level_menu.removeClass("lockheight");
	//use fragment to determine if an automatic "slide out" action of third level menu is needed
	var hash = window.location.hash;
	if(hash !== "#header") {
		//First disable css transition. Then hide the menu by changing style class. Finally re-enable transition and make a "slide out" move
		third_level_menu.addClass("no-animation");
		setTimeout(function() {
			third_level_menu.removeClass("no-animation");
		}, 300);
	}
	third_level_menu.append("<div id='leftmenu-controller'><div class='triangle-right'></div></div>");
	third_level_menu_arrow = $("#leftmenu-controller > div");
	$("#leftmenu-controller").click(toggleThirdLevelMenu);
}