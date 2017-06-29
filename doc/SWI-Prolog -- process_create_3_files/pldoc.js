/*  $Id$

    This file is  part  of  the   SWI-Prolog  PlDoc  package  for online
documentation browsing. It defines JavaScript to  issue HTTP requests on
the Prolog server that do not create a new page.

Author:  Jan Wielemaker & Michiel Hildebrand
Copying: Public domain
*/

var req;

function Initialize()
{ if ( req && req.readyState != 0 )
    req.abort();

  try
  { req = new ActiveXObject("Msxml2.XMLHTTP");
  } catch ( e )
  { try
    { req = new ActiveXObject("Microsoft.XMLHTTP");
    }
    catch(oc)
    { req=null;
    }
  }

  if ( !req && typeof XMLHttpRequest != "undefined" )
  { req = new XMLHttpRequest();
  }
}


function Process()
{ if ( req.readyState == 4 )
  { if ( req.status == 200 )
    { if ( req.responseText=="" )
      { //HideDiv("suggestionbox");
      } else
      { //document.getElementById("suggestionbox").innerHTML = req.responseText;
	//ShowDiv("suggestionbox");
      }
    }
  }
}


function HTTPrequest(url)
{ Initialize();

  if ( req != null )
  { req.onreadystatechange = Process;
    req.open("GET", url, true);
    req.send(null);
  } else
  { alert("Could not initialize HTTP request")
  }
}

/* Improve footnote interaction.  Contributed by Anne Ogborn.
*/

$(window).load(function(){
	var footnoteactivator = $('.fn');
	footnoteactivator.mouseenter(function() {
		window.clearTimeout(this.footnoteid);
		var fn = $(this).find('span.fn-text');
		if ( fn ) {
		  fn.removeClass('fn-text');
		  fn.addClass('fnp');
		}
		$(this).find('span.fnp').show(100);
	});
	footnoteactivator.mouseleave(function() {
		var t = $(this).find('span.fnp');
		if(this.footnoteid !== null)
		{
		    window.clearTimeout(this.footnoteid);
		}

		this.footnoteid = window.setTimeout(

			function() {
				  t.hide(100);
			}, 2000);
	});
});
