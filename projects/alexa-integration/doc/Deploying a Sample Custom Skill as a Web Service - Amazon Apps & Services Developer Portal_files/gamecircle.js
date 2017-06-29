// JavaScript Document
// Code tabs
$('.a-tabs li').click(
	function() {
		var languageList = $('.a-tabs li'), contentList = $('.a-box-tab'), displayedContent = 'tab-content-shown';
		// Set the selected language tab to active across all
		// content on the page
		var tabId = $(this).attr('id');
		languageList.removeClass('a-active');
		languageList.addClass(function() {
			if ($(this).attr('id') == tabId) {
				return 'a-active';
			}
		});
		// Only display the content associated with the selected
		// tab.
		// Do this for all content on the page, not just the current
		// selection.
		contentList.removeClass(displayedContent);
		var contentId = $(this).attr('id') + "-content";
		contentList.addClass(function() {
			if ($(this).attr('id') == contentId) {
				return displayedContent;
			}
		});
   });