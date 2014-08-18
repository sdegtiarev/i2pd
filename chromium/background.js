//chrome.webRequest.onBeforeSendHeaders.addListener(
chrome.webRequest.onBeforeRequest.addListener(
  function(details) {
	var addr=localStorage.getItem('addr');
	var port=localStorage.getItem('port');
	var redirect='http://'+addr+':'+port+'?'+details.url;
	return { redirectUrl: redirect};
  },
  { urls: ["*://*.i2p/*", ], },
  ["blocking"]
);


if(localStorage.getItem('addr') == null)
  localStorage.setItem('addr', '127.0.0.1');
if(localStorage.getItem('port') == null)
  localStorage.setItem('port', 7070);

