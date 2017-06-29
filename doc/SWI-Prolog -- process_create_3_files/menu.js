$(function() {
    $(".menubar-container > li").bind('mouseenter', function() {
        $(this).contents().filter('ul').addClass('activemenu');
        $(this).contents().filter('ul').offset({ top: $(this).offset().top + 31, left: $(this).offset().left });
        $(this).contents().filter('ul').css('position', 'absolute');
    }).bind('mouseleave', function() {
        var foo = this;
        window.setTimeout(function() {
            $(foo).contents().filter('ul').removeClass('activemenu');
        }, 100);
    });
    $(".menubar-container > li > ul").bind('mouseenter', function() {
        $(this).addClass('mouseovermenu');
    }).bind('mouseleave' , function() {
        var foo = this;
        window.setTimeout(function() {
            $(foo).removeClass('mouseovermenu');
        }, 100);
    });
    $(".menubar-container > li > ul > li").bind('mouseenter mouseleave', function() {
        $(this).toggleClass('mouseovermenuitem');
        $(this).contents().filter('ul').toggleClass('activemenu');
        $(this).contents().filter('ul').offset({ top: $(this).offset().top , left: $(this).offset().left + 208 });
        $(this).contents().filter('ul').css('position', 'absolute');
    });
    $(".menubar-container > li > ul > li > ul > li").bind('mouseenter mouseleave', function() {
        $(this).toggleClass('mouseovermenuitem');
    });
});