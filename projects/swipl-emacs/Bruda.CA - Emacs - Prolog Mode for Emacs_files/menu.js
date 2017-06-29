function angelmenu(){
	var z = 0;
	jQuery('#navcontainer li').has('ul').addClass('tab');
	
    jQuery('#navcontainer li.tab').hover(function(){
        var my_menu = jQuery('ul', this);  //can also be written as: var my_menu = jQuery(this).find('ul');		
        alert
        jQuery('a',this).css({
        	position:'relative',
        	zIndex:9999,
        	paddingBottom:'6px'
        });
        my_menu.fadeIn(200);
        my_menu.css('z-index','999')
        
    },function(){
    	var my_menu = jQuery('ul', this);  
    	my_menu.fadeOut(200);
    	jQuery('a',this).css({
    		position:'static',
    		zIndex:9999,
    		paddingBottom:'0px'
    	});
    });
	 

}
jQuery.noConflict(); 
 
jQuery(document).ready(function(){
    angelmenu();
});