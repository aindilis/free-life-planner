var GlobTitle = "";
var GlobTema = "";

function SetGlobals(title,tema)
{
	GlobTitle = title;
	GlobTema = tema;
}

function wr(s)
{
	document.write(s);
}

function wrp(s)
{
	document.write("<p>" + s + "</p>");
}

function CallUrl(url,nr,code) {
	var url2 = url.replace(/>/g,".");
	url2 = url2.replace(/</g,"/");	
	if (url2.indexOf("https://") == -1)	
	{
		url2 = "http://" + url2;
	}
	url3 = url2;
	url3 = url3.replace(/http:\/\//g,"");	
	url3 = url3.replace(/https:\/\//g,"");	

  wr('<span class="forrasurl"><a href="' + url2 + '" target="_blank" rel="nofollow">['+shorturl(url3)+']</a></span>'); 
}

function TildeLink()
{
	wr('<sup>[<a href="#maintitle" class="hiv">&uArr;</a>]</sup>');
}

function shorturl(url)
{
    if (url.length>60) 
    {
        var lastidx = url.substring(0,60).lastIndexOf("/");
        return url.substring(0,lastidx+1)+"...";
    }
	else
	{
		return url;
	}		
}

function ShareIt(title,tema,descr)
{
	var urlfb = "https://www.facebook.com/sharer/sharer.php?u=" + window.location.href;
	var urltw = "https://twitter.com/intent/tweet?text=" + title + "-" +descr + "&url=" + window.location.href + "&hashtags=" + tema;
	wr('<a href="'+urlfb+'" target="_blank" rel="nofollow"><img src="/images/fbshare.gif" alt="Share Facebook" width="32" height="32" hspace="2" border="0"></a>');
	wr('<a href="'+urltw+'" target="_blank" rel="nofollow"><img src="/images/twshare.gif" alt="Share Twitter"  width="32" height="32" hspace="2" border="0"></a>');
}
function MimiF1()
{
	var imagepath = "/images/act.gif";

	wr('<br><div class="bmenucim">&nbsp;&nbsp;' + LangTopics + '</div><br>');
	
	wr('<table width="100%" border="0" cellspacing="0" cellpadding="0" class="bmenu">');

    var i;
	for (i=0; i<tema.length; i+=2)  
	{
        var dnev = tema[i+1];
        if (dnev=="") dnev = tema[i].toLowerCase();

		wr('<tr>');
		if (tema[i] == GlobTema)
		{
			wr('<td width="15"><img src="' + imagepath + '" width="7" height="14"></td>');
		}
		else
		{
			wr('<td width="15"></td>');
		}
	

		wr('<td><a href="/' + dnev + '/index_' + dnev + '.html"  class="bmenu">' + tema[i] + '</a></td>');		
        wr('</tr>');
	}	
	
	wr('</table>');
    UjSor(UjSorCnt);
}	

function UjSor(nr)
{
   //for (i=0; i<nr; i++) { wr('<br>&nbsp;'); }
}

function Menu()
{
	var i;			
	for (i = 1; i < MainMenu.length; i+=2)  
	{
		if (String(MainMenu[i]).indexOf("http://") != 0)
		{
			MainMenu[i] = "/" + MainMenu[i];
		}
	}			

    var space = '&nbsp;&nbsp; ';
    wr(space); 
	wr('<a href="/topics.html" class="fmenu"'+target+'>' + LangTopics + '</a>'+space);
	for (i = 0; i < MainMenu.length; i+=2)  
	{
        var target="";
        if (String(MainMenu[i+1]).indexOf("http://") == 0)
        {
            target=" target=_blank";
        }
        if (MainMenu[i]=="$") { wr('<br>'); continue; }        
		wr('<a href="' + MainMenu[i+1] + '" class="fmenu"'+target+'>' + MainMenu[i] + '</a>'+space); 
	}			
}

function MkTemaOpt()
{
    var ss='<option value="">'+LangSelectTopic+'</option>';
    for (i=0; i<tema.length; i+=2)
	{
        var s = tema[i].substring(0,19);
		ss+='<option value="'+tema[i]+'">'+s+'</option>';
	}
    return ss;
}

function Leiras()
{
    wr('<div class=kisbetu><i>'+LangFelelosseg+'</i></div>');
}

function JobbMenu1()
{
	JobbMenuJavaslat();	
	JobbMenuVelemeny();
}

function keret(fejlec,txt)
{
  var s = "";
  if (fejlec != "") s += '<div class=jboxfej>'+fejlec+'</div><br>';
  s += txt;
  wr('&nbsp;<br><table width="160" border="0" cellspacing="0" cellpadding="0"><tr>');
	  wr('<td width="10">&nbsp;</td>');
	  wr('<td style="border:1px solid #999999; background-color: #FFFFFF; padding:7px; border-radius:10px; text-align:center;">'+s+'</td>');
	  wr('<td width="10">&nbsp;</td>');
  wr('</tr></table>');
}

function JobbMenuJavaslat()
{
	var s='';
	s+='<form class=kisbetu method="POST" action="/linksuggest_'+LandCode+'.php">';
	if (LangSuggestWebPageUrl!="") s+=LangSuggestWebPageUrl+'<br>';
	s+='<input name="url" value="http://" size="20" type="text" class="kisbetu"><br>';
	s+='<br>'+LangSuggestWebPageDescr+'<br>';
	s+='<textarea name="leiras" cols="16" rows="6" class="kisbetu"></textarea><br>';
	s+='<br>'+LangTopic+':<br>';
    s+='<select name="tema" class="kisbetu" size="1">';    
    s+=MkTemaOpt();
    s+='</select><br>';
	s+='<input class="kisbetu" type="submit" value="'+LangSubmit+'">';
	s+='</form>';		
    keret(LangSuggestWebPage,s);
}

function JobbMenuVelemeny()
{
    var s='';
	s+='<form class=kisbetu method="POST" action="/velemeny.php">'+LangYourOpinionDescr;
	s+='<textarea class="kisbetu" name="velemeny" cols="17" rows="6"></textarea>';
	s+='<br><br>'+LangYourOpinionEmail+'<br>';
	s+='<input class="kisbetu" type="text" size="20" name="emailcim"><br>';		  
	s+='<input class="kisbetu" type="submit" value="'+LangSubmit+'">';
	s+='</form>';
    keret(LangYourOpinion,s);
}
function ReplaceCallUrl(id,url) {

	// change in mobil version !!! 
    var container = document.getElementById(id);
	var url1 = url.replace(/>/g,".");
	url1 = url1.replace(/</g,"/");
    var url2 = "http://" + url1;
    var url3 = url2;

    if (url3.length>80) 
    {
        var lastidx = url3.substring(0,80).lastIndexOf("/");
        url3 = url3.substring(0,lastidx+1)+"...";
    }

    var s = '<span class="forrasurl">&gt;&gt;&gt; <a href="' + url2 + '" target="_blank" rel="nofollow">'+ url3 +'</a></span>';
    container.innerHTML += "<br>" + s;
}

var LangTopic = "Topic";
var LangTopics = "Topic";
var LangHome = "Home";
var LangSelectTopic = "Select topic";
var LangUrlName = "Full article"; // ures, ha URL direkt
var LangSubmit = "Submit";
var LangSuggestWebPage = "Suggestion";
var LangSuggestWebPageUrl = "Suggest an interesting web page:";
var LangSuggestWebPageDescr = "Description:";
var LangYourOpinion = "Your opinion";
var LangYourOpinionDescr = "";
var LangYourOpinionEmail = "e-mail: (optional)";
var LangFelelosseg = "";

var LandCode = "en";
var UjSorCnt = 10;

var AdvList = new Array();


var tema = new Array(    
    "Animals","",
	"Antiques","",
    "Aquarium","",
    "Architecture","",
    "Artificial Intelligence","artificial_intelligence",
    "Astrology","",
    "Astronomy","",
    "Aviation","",
    "Basketball","",
    "Beauty","",
    "Biology","",
    "Boating","",
    "Bonsai","",
    "Business","",
    "Career","",
    "Cat","",
    "Diet","",
    "Dog","",
    "Dreams","",
    "Environment","",
    "Esoteric","",
    "Fine arts","finearts",
    "Fitness","",
    "Gardening","",
    "Gastronomy","",
    "GIS","",
    "Golf","",
	"Heart & Circulation","heart-circulation",
    "Home","",
    "Horse","",
    "Jewelry","",
    "Law","",
    "Marketing & Web","marketingweb",
    "Mathematics","",
    "Meteorology","",
    "Medicine","",    
    "Military","",        
    "Model Railroad","modelrailroad",
    "Numismatic","",
    "Philately","",
    "Photography","",
	"Plants","",
    "Pregnancy & Parenting","pregnancy",
    "Statistics","",
    "Stock market","stockmarket",
    "Wine","",
    "Yoga",""  
);
	

var MainMenu = new Array(    
    "The team", "theteam.html", 
    "Your opinion","opinion.html",            
    "Contact us","opinion.html",
    "Privacy policy","policy.html"
);
	

//********************************
//********************************	
// atirt funkciok


//********************************
//********************************	
// extra funkciok

