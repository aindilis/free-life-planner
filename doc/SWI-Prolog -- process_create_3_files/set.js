// -------------------------------------------------------------------
// markItUp!
// -------------------------------------------------------------------
// Copyright (C) 2008 Jay Salvat
// http://markitup.jaysalvat.com/
// -------------------------------------------------------------------
// MarkDown tags example
// http://en.wikipedia.org/wiki/Markdown
// http://daringfireball.net/projects/markdown/
// -------------------------------------------------------------------
// Feel free to add more tags
// -------------------------------------------------------------------
pldoc_settings = {
	previewParserPath:	'/markitup/preview/pldoc',
	onShiftEnter:		{keepDefault:false, openWith:'\n\n'},
	markupSet: [
		{name:'Heading 1', key:'1', openWith:'# ', placeHolder:'Your title here...' },
		{name:'Heading 2', key:'2', openWith:'## ', placeHolder:'Your title here...' },
		{name:'Heading 3', key:'3', openWith:'### ', placeHolder:'Your title here...' },
		{separator:'---------------' },
		{name:'Bold',   key:'B', openWith:'*', closeWith:'*'},
		{name:'Italic', key:'I', openWith:'_', closeWith:'_'},
		{name:'Code',   key:'C', openWith:'=', closeWith:'='},
		{separator:'---------------' },
		{name:'Bulleted List', openWith:'  - ', multiline:true },
		{name:'Numeric List', multiline:true, openWith:function(markItUp) {
			return "  "+markItUp.line+'. ';
		}},
		{separator:'---------------' },
//		{name:'Picture', key:'P', replaceWith:'![[![Alternative text]!]]([![Url:!:http://]!] "[![Title]!]")'},
		{name:'Link', key:'L', openWith:'[', closeWith:']([![Url:!:http://]!])', placeHolder:'Your text to link here...' },
		{separator:'---------------'},
		{name:'Code Block', openBlockWith:'  ~~~~\n', closeBlockWith:'\n  ~~~~', openWith:'  '},
		{separator:'---------------'},
		{name:'Preview', call:'preview', className:"preview"},
		{ name:'Hide Preview',
		  className:"hidepreview",
		  call:function(markitUp){ miu.hidepreview(); }
		}
	]
}

// mIu nameSpace to avoid conflict.
miu = {	hidepreview : function() {
		$('.markItUpPreviewFrame').remove();
	}
}
