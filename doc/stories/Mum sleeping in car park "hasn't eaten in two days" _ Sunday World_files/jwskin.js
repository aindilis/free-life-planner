if (typeof custom_playlist_n == 'undefined')
{
   var custom_playlist_n = 1;
}

if (typeof custom_playlist_number == 'undefined')
{
   var custom_playlist_number = 0;
}

var inc_playlist = 0;

if (document.getElementById('custom_listbar' + custom_playlist_n))
{
   inc_playlist = 1;	
}

var output_for_skin = '<style> .custom-listbar-row { padding: 10px; background: #3c3c3e; border-bottom: 1px solid #2f2f31; cursor: pointer; color:#d4d3d5; font-family: Arial, sans-serif, "Helvetica Neue", Helvetica; font-size:12px; font-weight:bold; } .custom-listbar-row:hover { color:#FFF; } .custom-listbar-row.active { background: #2f2f31; color:#FFF; } .custom-listbar-row:last-child { border-bottom: 0; } .custom-listbar-row img { width: 80px; margin-right: 20px; } .custom-listbar-row span, img { display:inline-block; vertical-align: middle; } </style><div id="custom_listbar'+(custom_playlist_n + inc_playlist) +'"></div>';

document.write(output_for_skin);

var _bind = function(el, evt, fcn) {
    el[window.addEventListener ? 'addEventListener' : 'attachEvent']( window.addEventListener ? evt : 'on' + evt, fcn, false);
}

jwplayer(custom_playlist_number ++).onPlaylist(function(e) {
	var player = jwplayer(),
		playlist = e.playlist || [],
		listbar = document.getElementById('custom_listbar' + custom_playlist_n);
		listbar.style.width = player.getWidth() + 'px';
		custom_playlist_n ++;
	for(var k = 0; k < playlist.length; k++) {        
		var item = playlist[k],
			row = document.createElement('div');
		row.className = k === 0 ? 'custom-listbar-row active' : 'custom-listbar-row';
		row.innerHTML = '<span>' +  item.title + '</span>';
		(function(itemIdx, row) {
			_bind(row, 'click', function() {
				var rows = listbar.children;                
				for(var r = 0; r < rows.length; r++) {
					rows[r].className = 'custom-listbar-row';
				}
				player.playlistItem(itemIdx);
				row.className += ' active';
			});
		})(k, row);

		listbar.appendChild(row);
	}
}); 

window.onload = function()
{
	var jwplayer_firefox_fix = window.setInterval(function()
	{
	   var player = jwplayer();
	   if (typeof player.getWidth() != 'undefined')
	   {
		   var listbar = document.getElementById('custom_listbar' + custom_playlist_n);
		   if (listbar)
		   {
			  listbar.style.width = player.getWidth() + 'px';
		      window.clearInterval(jwplayer_firefox_fix);
		   }
	   }
	}, 100);
}
