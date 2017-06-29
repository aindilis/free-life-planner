var tbt = document.createElement('DIV');
tbt.innerHTML = '<div id="tooltipBox" onMouseOver="clearAdInterval();" onMouseOut="hideAd();" style="padding:0px;text-align:left;background-position: left; z-index:2147483647;position:absolute;cursor:pointer;"></div>';
document.body.appendChild(tbt);

try { ctx_LinkColor == ""; } catch(e) { ctx_LinkColor = "009900"; }

var sty = document.createElement('STYLE');
sty.innerHTML = '<style type="text/css">SPAN.CTXempty {text-decoration: inherit !important; font-face: inherit; float: none !important;} SPAN.CTXStyle{ margin: 0px !important; text-decoration: underline !important; cursor: pointer !important; cursor: hand !important; padding-bottom: 1px !important; color: #' + ctx_LinkColor + ' !important; border-bottom: 1px solid #' + ctx_LinkColor + ' !important; float: none !important; display:inline !important;}</style>';
document.body.appendChild(sty);

function start_CTX()
{
var turl2='//lb.c-on-text.com/pagesCache.js';
var script2=document.createElement('script');
script2.type='text/javascript';
script2.src=turl2;
//document.getElementsByTagName('head')[0].appendChild(script2);

//if (window.location.href.indexOf('independent.ie/business') > -1) return; 
if (window.location.href.indexOf('sundayworld.com/') > -1) return;
if (window.location.href.indexOf('sundayworld.com/news') > -1) return;
if (window.location.href.indexOf('globo.com/') > -1) return;
try
  {
    try { blockCount++; } catch(e) { blockCount = 1; }
    var turl='//engine3.c-on-text.com/getads.aspx?stype=script&siteGUID='+ctx_SiteID+'&blockCount='+blockCount+'&refurl='+document.location.href.replace(/\&/g,'%26').replace(/\'/g,'%27').replace(/\"/g,'%22')+'&reffurl='+document.referrer.replace(/\&/g,'%26').replace(/\'/g,'%27').replace(/\"/g,'%22');
    var script=document.createElement('script');
    script.type='text/javascript';
    script.src=turl;
    document.getElementsByTagName('head')[0].appendChild(script);
    if (document.location.href.indexOf('ctxjsdebug=true') > 0) 
    { 
      var tdiv= document.createElement('DIV');
      tdiv.innerHTML = '<span onclick="document.getElementById(\'debugDIV\').style.display=\'block\';" style="cursor:pointer;">.</span><div id="debugDIV" style="display:none;"></div>';
      document.body.appendChild(tdiv);
      debugMsg('script initiated');
    }
  }
  catch(e){}
}

function initScript()
{
  var t=setTimeout('start_CTX()',100);
}


function checkTime(i)
{
  if (i<10) { i="0" + i; }
  return i;
}

function checkTimeMsec(i)
{
  if (i<100 && i>10) { i="0" + i; }
  else if (i<10) { i="00" + i; }
  return i;
}

function debugMsg(s)
{
  var dbg = document.getElementById('debugDIV');
  if (dbg != null)
  {
    var d = new Date();
    dbg.innerHTML = dbg.innerHTML + checkTime(d.getHours()) + ":" + checkTime(d.getMinutes()) + ":" + checkTime(d.getSeconds()) + "." + checkTimeMsec(d.getMilliseconds()) + " - " + s + "<br>";
  }
}

addDOMLoadEvent=(function(){var e=[],t,s,n,i,o,d=document,w=window,r='readyState',c='onreadystatechange',x=function(){n=1;clearInterval(t);while(i=e.shift())i();if(s)s[c]=''};return function(f){if(n)return f();if(!e[0]){d.addEventListener&&d.addEventListener("DOMContentLoaded",x,false);/*@cc_on@*//*@if(@_win32)d.write("<script id=__ie_onload src=//0><\/scr"+"ipt>");s=d.getElementById("__ie_onload");s[c]=function(){s[r]=="complete"&&x()};/*@end@*/if(/WebKit/i.test(navigator.userAgent))t=setInterval(function(){/loaded|complete/.test(d[r])&&x()},10);o=w.onload;w.onload=function(){x();o&&o()}}e.push(f)}})();
addDOMLoadEvent(initScript);