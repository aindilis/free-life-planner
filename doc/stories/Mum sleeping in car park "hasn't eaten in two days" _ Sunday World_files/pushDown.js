(function () {

    var newElement;
    var newElementIFrame;
    var adData;
    var interval;
    var statsIncrementURL = "http://estadaoapi.c-on-text.com/api/StatsIncrement";
    
    
    function setupElements() {
      loadFont();
      var targetArea = document.querySelector(ctx_pd.slot.split('>')[0].trim());
      if (targetArea) {
          var members = document.querySelectorAll(ctx_pd.slot);
            if (parent.location.href.toLowerCase().indexOf('epocanegocios.globo.com/caminhos-para-o-futuro') > -1 || parent.location.href.toLowerCase().indexOf('revistagalileu.globo.com/caminhos-para-o-futuro') > -1) 
            { 
              targetArea = targetArea.children[0];
              members = targetArea.getElementsByTagName('p');
            }
//          if (members.length > 0) {
              newElement = document.createElement('DIV');
              newElement.style.height = "0px";
              newElement.style.overflowY = "hidden";
              newElement.style.textAlign = "center";
              newElement.style.transition = 'height 500ms';
              newElement.style.clear = 'both';
              if (ctx_pd.cssText) newElement.style.cssText += ctx_pd.cssText;
              if (members.length > ctx_pd.pos) { targetArea.insertBefore(newElement, members[ctx_pd.pos]); }
              else { targetArea.appendChild(newElement); }
//          }
//          else { console.log('C|ON|TEXT PushDown: Target area contains no child objects of specified type'); }
      }
      else { console.log('C|ON|TEXT PushDown: Target area not found'); }
    }

    function loadFont() {
        WebFontConfig = {
            google: { families: ['Lato:700italic:latin'] }
        };
        var wf = document.createElement('script');
        wf.src = 'https://ajax.googleapis.com/ajax/libs/webfont/1/webfont.js';
        wf.type = 'text/javascript';
        wf.async = 'true';
        var s = document.getElementsByTagName('script')[0];
        s.parentNode.insertBefore(wf, s);
    }

    function tick() {
        var middle = newElement.getBoundingClientRect().top + (newElementIFrame.clientHeight / 2);
        if (middle > 0 && middle < window.innerHeight) {
            newElement.style.height = newElementIFrame.clientHeight + "px";
	    if (adData[0].Title.indexOf('youtube_autoplay') > -1) 
            { 
              var pushDownFrame = document.getElementById("pushDownFrame");
              var playButton = pushDownFrame.contentWindow.document.getElementById("playButton");
              playButton.click();
            }
            clearInterval(interval);
	    if (adData[0].Title.indexOf('external_ad') > -1) getPostData("POST", statsIncrementURL, JSON.stringify([{CampaignID: adData[0].CampaignID, Activations: 1 }]), null);
        }
    }

    function getPostData(method, url, data, callback) {
        var xhr = new XMLHttpRequest();
        if (callback != null) {
            xhr.addEventListener("readystatechange", function () {
                if (this.readyState === this.DONE) {
                    callback(JSON.parse(this.responseText));
                }
            });
        }

        xhr.open(method, url);
        xhr.setRequestHeader("cache-control", "no-cache");
        xhr.setRequestHeader("content-type", "application/json; charset=utf-8");
        xhr.setRequestHeader("accept", "application/json; charset=utf-8");
        xhr.send(data);
    };

    function getPageData() {
        var pageData = new Object();

        pageData.url = parent.location.href;
        //pageData.url = '//service.wellclever.com/casestudies/EstadaoApi20150911/Discovery/index.htm';
        pageData.siteID = ctx_pd.siteID;
	if (pageData.siteID == null || pageData.siteID == '') pageData.siteID = ctx_pd.siteId;
        pageData.referrer = parent.document.referrer;
        pageData.userAgent = navigator.userAgent;
        pageData.screenWidth = screen.width;
        pageData.screenHeight = screen.height;

        return pageData;
    };

    function handleSuccess(data) {
        //console.log(JSON.stringify(data, "", "    "));
        adData = data;
        if (adData.length > 0) showAd(0);
    };

    function showAd(index) {
	setupElements();
        var Html = getAdHtml(index);
        newElement.innerHTML = '<iframe id="pushDownFrame" frameborder="no" scrolling="no" allowtransparency="true" style="z-index:2147483647;"></iframe>';
        newElementIFrame = document.getElementById('pushDownFrame');
        newElementIFrame.contentWindow.document.open();
        newElementIFrame.contentWindow.document.write('<html><head></head><body style="background-color:transparent;margin:0px; padding:0px;">' + Html + '</body></html>');
        newElementIFrame.contentWindow.document.close();
        newElementIFrame.style.height = (newElementIFrame.contentWindow.document.getElementById('ctxTab').offsetHeight) + "px";
        newElementIFrame.style.width = (newElementIFrame.contentWindow.document.getElementById('ctxTab').offsetWidth) + "px";
        interval = window.setInterval(tick, 100);

	if (adData[0].Title.indexOf('external_ad') > -1) 
		getPostData("POST", statsIncrementURL, JSON.stringify([{CampaignID: adData[0].CampaignID, Impressions: 1 }]), null);
    }

    window.ctx_pd_closeAd = function () {
        newElement.addEventListener("transitionend", function (event) { newElement.innerHTML = ''; }, false)
        newElement.style.height = "0px";
    }

    function getAdHtml(index) {
        var TH = "";
        TH += getAdHeader(index);
        switch (adData[index].AdType) {
            case "image": TH += getAdImage(index); break;
            case "video": TH += getAdVideo(index); break;
            case "text": if (adData[index].Title.indexOf('external_ad') > -1) TH += getAdExternalDefault(index); break;
            default: break;
        }
        TH += getAdFooter(index);

        return TH;
    }

    function getAdHeader(index) {
        var TH = "";
        TH += '<table id="ctxTab" name="ctxTab" border="0" cellpadding="0" cellspacing="0" style="background-color:transparent; padding-bottom: 20px;">';
        TH += '<tr>';
        TH += '<td>';
        TH += '<div style="position:relative;font-family: \'Lato\', sans-serif; font-size:10px;line-height:16px;">' + (adData[index].Title.indexOf("publicidade_caption") > -1 ? "PUBLICIDADE": "ADVERTISEMENT") + '<div style="position:absolute; right:0px; top:0px; cursor:pointer;font-size:16px;" onclick="parent.ctx_pd_closeAd();">×</div></div>';
        //TH += '<div style="box-shadow:2px 2px 10px 0px rgba(0, 0, 0, 0.75);">';
        return TH;
    }

    function getAdFooter(index) {
        var TH = "";
        //TH += '</div>';
        if (adData[index].Title.indexOf("powered_by_wellclever") > -1) TH += '<div style="text-align:right; line-height:16px; font-size:12px; font-family: \'Lato\', sans-serif;">Powered by Wellclever</div>';
        TH += '</tr>';
        TH += '</table>';
        return TH;
    }

    function getAdImage(index) {
        var ratio = newElement.offsetWidth / adData[index].ImageWidth;
        var w = ratio < 1 ? Math.floor(adData[index].ImageWidth * ratio) : adData[index].ImageWidth;
        var h = ratio < 1 ? Math.floor(adData[index].ImageHeight * ratio) : adData[index].ImageHeight;
        var TH = "";
        TH += '<div onclick="window.open(\'' + adData[index].Url + '\');" style="cursor: pointer; position: absolute; float: left; width: ' + w + 'px; height: ' + h + 'px; background-color:#FFFFFF; filter: progid:DXImageTransform.Microsoft.Alpha(opacity=1); opacity: 0.01;"></div>';
        TH += '<img src="' + adData[index].ImageURL + '" width="' + w + '" height="' + h + '" border="0">';
        if (adData[index].Title.indexOf('ImpressionTracker') > -1) TH += '<img src="' + adData[index].Body.replace('<BR>', '') + '" width="1" height="1" border="0" style="position: absolute; float: left;">';
        return TH;
    }

    function getAdVideo(index) {
        if (adData[index].Title.indexOf('external_ad') > -1) { return getAdVideoYouTube(index); }
        else { return getAdVideoHTML5(index); }
    }

    function getAdVideoHTML5(index) {
        var ratio = newElement.offsetWidth / adData[index].ImageWidth;
        var w = ratio < 1 ? Math.floor(adData[index].ImageWidth * ratio) : adData[index].ImageWidth;
        var h = ratio < 1 ? Math.floor(adData[index].ImageHeight * ratio) : adData[index].ImageHeight;
        var TH = "";
        TH += '<script>';
        TH += 'function init() { ';
        TH += 'var videoPreview = document.getElementById("videoPreview"); var videoContent = document.getElementById("videoContent"); var playButtonBig = document.getElementById("playButtonBig"); var playButton = document.getElementById("playButton"); var tracker = document.getElementById("tracker"); var trackerBuffered = document.getElementById("trackerBuffered"); var trackerBackground = document.getElementById("trackerBackground"); var muteButton = document.getElementById("muteButton"); var clickIcon = document.getElementById("clickIcon");';
        TH += 'playButtonBig.addEventListener("click", playStop); playButton.addEventListener("click", playStop); muteButton.addEventListener("click", muteUnmute); videoContent.addEventListener("timeupdate", timeupdate); videoContent.addEventListener("progress", progress); videoContent.addEventListener("click", adClicked); videoContainer.addEventListener("mouseover", showControls); videoContainer.addEventListener("mouseout", hideControls); clickIcon.addEventListener("click", adClicked);';
        TH += 'function playStop() { if (videoContent.paused == true) { videoContent.play(); videoPreview.style.display = "none"; playButtonBig.style.display = "none"; playButton.style.backgroundImage = "url(\'//www.c-on-text.com/images/video/btnPause.png\')"; showControls(); } else { videoContent.pause(); playButton.style.backgroundImage = "url(\'//www.c-on-text.com/images/video/btnPlay.png\')"; } }';
        TH += 'function muteUnmute() { if (videoContent.muted == false) { videoContent.muted = true; muteButton.style.backgroundImage = "url(\'//www.c-on-text.com/images/video/btnMute.png\')"; } else { videoContent.muted = false; muteButton.style.backgroundImage = "url(\'//www.c-on-text.com/images/video/btnSound.png\')"; } } ';
        TH += 'function timeupdate() { var elapsedPct = videoContent.currentTime / videoContent.duration; tracker.style.width = Math.round(trackerBackground.clientWidth * elapsedPct) + "px"; if (videoContent.currentTime == videoContent.duration) { playButton.style.backgroundImage = "url(\'//www.c-on-text.com/images/video/btnPlay.png\')"; videoPreview.style.display = "block"; } } ';
        TH += 'function progress() { if (videoContent.buffered.length > 0) { var bufferedPct = videoContent.buffered.end(0) / videoContent.duration; trackerBuffered.style.width = Math.round(trackerBackground.clientWidth * bufferedPct) + "px"; } }';
        TH += 'function showControls() { videoControls.style.opacity = .8; clickIcon.style.bottom = "40px"; }';
        TH += 'function hideControls() { videoControls.style.opacity = 0; clickIcon.style.bottom = "5px"; }';
        TH += 'function adClicked() { window.open(\'' + adData[index].Url + '\'); }';
        TH += '}';
        TH += 'document.addEventListener("DOMContentLoaded", init, false);';
        TH += '</script>';
        TH += '<div id="videoContainer" style="width: ' + w + 'px; height: ' + h + 'px; position: relative;">';
        TH += '<div id="clickIcon" style="display: ' + (adData[index].Title.indexOf('click_icon') > -1 ? 'block' : 'none')  + '; cursor: pointer; position:absolute; bottom:5px; right:5px; height:37px; width: 37px ; transition: bottom .3s; background-color: #000000; color:#FFFFFF; opacity:0.3; z-index:5; text-align: left; vertical-align:middle;"><img src="//www.c-on-text.com/images/click-icon.png" /></div>';
        TH += '<div id="videoPreview" style="width: ' + w + 'px; height: ' + h + 'px; background-image: url(\'' + adData[index].ThumbURL + '\'); background-size: 100%; position: absolute; z-index: 2;"></div>';
        TH += '<video id="videoContent" width="' + w + '" height="' + h + '" preload="none" style="width: ' + w + 'px; height: ' + h + 'px; cursor: pointer; position: absolute; z-index: 1;">';
        TH += '<source src="' + adData[index].ImageURL + '" type="video/mp4">';
        TH += '</video>';
        TH += '<div id="playButtonBig" style="left: ' + Math.round((w - 100) / 2) + 'px; top: ' + Math.round((h - 100) / 2) + 'px; width: 100px; height: 100px; position: absolute; cursor: pointer; z-index: 4; background-image: url(\'//www.c-on-text.com/images/video/btnPlayBig.png\');"></div>';
        TH += '<div id="videoControls" style="width: ' + (w - 10) + 'px; height: 30px; bottom: 5px; left: 5px; background-color: #333333; position: absolute; opacity: 0; -webkit-transition: opacity .3s; -moz-transition: opacity .3s; -o-transition: opacity .3s; -ms-transition: opacity .3s; transition: opacity .3s; z-index: 3; -webkit-border-radius: 5px; -moz-border-radius: 5px; border-radius: 5px;">';
        TH += '<div id="playButton" style="height: 30px; width: 30px; left: 0px; bottom: 0px; cursor: pointer; position: absolute; background: url(\'//www.c-on-text.com/images/video/btnPlay.png\') no-repeat center;"></div>';
        TH += '<div id="trackerBackground" style="width: ' + (w - 70) + 'px; background-color: black; left: 30px; top: 13px; height: 6px; position: absolute; z-index: 1; -webkit-border-radius: 3px; -moz-border-radius: 3px; border-radius: 3px;"></div>';
        TH += '<div id="trackerBuffered" style="background-color: #555555; left: 30px; width: 0px; top: 13px; height: 6px; position: absolute; z-index: 2; -webkit-border-radius: 3px; -moz-border-radius: 3px; border-radius: 3px;"></div>';
        TH += '<div id="tracker" style="background-color: white; left: 30px; width: 0px; top: 13px; height: 6px; position: absolute; z-index: 3; -webkit-border-radius: 3px; -moz-border-radius: 3px; border-radius: 3px;"></div>';
        TH += '<div id="muteButton" style="height: 30px; width: 30px; right: 0px; bottom: 0px; cursor: pointer; position: absolute; background: url(\'//www.c-on-text.com/images/video/btnSound.png\') no-repeat center;"></div>';
        TH += '</div>';
        TH += '</div>';
        if (adData[index].Title.indexOf('ImpressionTracker') > -1) TH += '<img src="' + adData[index].Body.replace('<BR>', '') + '" width="1" height="1" border="0" style="position: absolute; float: left;">';
        return TH;
    }


  function getAdVideoYouTube(index)
  {
        var ratio = newElement.offsetWidth / adData[index].ImageWidth;
        var w = ratio < 1 ? Math.floor(adData[index].ImageWidth * ratio) : adData[index].ImageWidth;
        var h = ratio < 1 ? Math.floor(adData[index].ImageHeight * ratio) : adData[index].ImageHeight;
    var TH="";
        TH += '<script>';
        TH += 'function init() { ';
        TH += 'var videoPreview = document.getElementById("videoPreview"); var videoContent = document.getElementById("videoContent"); var playButton = document.getElementById("playButton"); var playTracker = document.getElementById("playTracker");';
	TH += 'playButton.addEventListener("click", playVideo);';
	TH += "function playVideo() { videoPreview.style.display='none'; playButton.style.display='none'; videoContent.innerHTML='" + adData[index].Body + "'; playTracker.src='" + adData[index].ImageURL + "'; }";
        TH += '}';
        TH += 'document.addEventListener("DOMContentLoaded", init, false);';
        TH += '</script>';
        TH += '<div style="width: ' + w + 'px; height: ' + h + 'px;">';
	TH += '<button id="playButton" style="position:absolute; z-index:2; width:68px; height:48px; left:50%; top:50%; margin-left:-34px; margin-top:-24px; cursor:pointer; border:none; background-color:transparent; padding:0">';
	TH += '<svg height="100%" version="1.1" viewBox="0 0 68 48" width="100%"><path class="ytp-large-play-button-bg" d="m .66,37.62 c 0,0 .66,4.70 2.70,6.77 2.58,2.71 5.98,2.63 7.49,2.91 5.43,.52 23.10,.68 23.12,.68 .00,-1.3e-5 14.29,-0.02 23.81,-0.71 1.32,-0.15 4.22,-0.17 6.81,-2.89 2.03,-2.07 2.70,-6.77 2.70,-6.77 0,0 .67,-5.52 .67,-11.04 l 0,-5.17 c 0,-5.52 -0.67,-11.04 -0.67,-11.04 0,0 -0.66,-4.70 -2.70,-6.77 C 62.03,.86 59.13,.84 57.80,.69 48.28,0 34.00,0 34.00,0 33.97,0 19.69,0 10.18,.69 8.85,.84 5.95,.86 3.36,3.58 1.32,5.65 .66,10.35 .66,10.35 c 0,0 -0.55,4.50 -0.66,9.45 l 0,8.36 c .10,4.94 .66,9.45 .66,9.45 z" fill="#1f1f1e" fill-opacity="0.81"></path><path d="m 26.96,13.67 18.37,9.62 -18.37,9.55 -0.00,-19.17 z" fill="#fff"></path><path d="M 45.02,23.46 45.32,23.28 26.96,13.67 43.32,24.34 45.02,23.46 z" fill="#ccc"></path></svg>';
	TH += '</button>';
        TH += '<img id="videoPreview" style="position:absolute; z-index:1;" src="' + adData[index].ThumbURL + '" width="' + w + '" height="' + h + '" border="0">';
	TH += '<div id="videoContent" style="position:absolute; width: ' + w + 'px; height: ' + h + 'px;"></div>';
        TH += '<img id="playTracker" style="display:none;">';
	TH += '</div>';
    return TH;
  }

    function getAdExternalDefault(index) {
        var ratio = newElement.offsetWidth / adData[index].ImageWidth;
        var w = ratio < 1 ? Math.floor(adData[index].ImageWidth * ratio) : adData[index].ImageWidth;
        var h = ratio < 1 ? Math.floor(adData[index].ImageHeight * ratio) : adData[index].ImageHeight;
        var TH = "";
        TH += '<div style="width: ' + w + 'px; height: ' + h + 'px;">';
        TH += adData[index].Body;
        TH += '</div>';
        return TH;
    }

    var turl = '//lb.c-on-text.com/pagesCache.js';
    var script = document.createElement('script');
    script.type = 'text/javascript';
    script.src = turl;
    //document.getElementsByTagName('head')[0].appendChild(script);

    if (parent.location.href.indexOf('globo.com/') > -1 && !(parent.location.href.toLowerCase().indexOf('epocanegocios.globo.com/caminhos-para-o-futuro/') > -1) && !(parent.location.href.toLowerCase().indexOf('epocanegocios.globo.com/tecnologia/') > -1) && !(parent.location.href.toLowerCase().indexOf('revistagalileu.globo.com/caminhos-para-o-futuro/') > -1) && !(parent.location.href.toLowerCase().indexOf('revistagalileu.globo.com/tecnologia/') > -1) && !(parent.location.href.toLowerCase().indexOf('revistagalileu.globo.com/ciencia/') > -1)) return;    
    //if (parent.location.href.indexOf('vogue.globo.com/mundo-vogue/promovogue/') > -1) return;    
    //if (parent.location.href.indexOf('vogue.globo.com/fno/') > -1) return;    
    if (parent.location.href.indexOf('sundayworld.com/christmas') > -1) return;    
    if (parent.location.href.indexOf('sundayworld.com/news') > -1) return;    
    if (parent.location.href.indexOf('m.aws-test.independent.ie') > -1) return;    
    if (parent.location.href.indexOf('m.independent.ie') > -1) return;    

    getPostData("POST", "//estadaoapi.c-on-text.com/api/PageData", JSON.stringify(getPageData()), handleSuccess);

})();
