function scrollTo(id) {
	id = '#' + id;
	var offsetTop = jQuery(id).offset().top - 90;
	jQuery("html,body").animate({scrollTop:offsetTop}, 'fast');
	
    return false;
}