const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html> 
<html>

<head>
<meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">
<title>Sea container weight measurement</title>

<style>
	html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
	body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}
	.button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 20px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}
	.button-on {background-color: #3498db;}
	.button-on:active {background-color: #2980b9;}
	.button-off {background-color: #34495e;}
	.button-off:active {background-color: #2c3e50;}
	p {font-size: 14px;color: #888;margin-bottom: 10px;}
 </style>
 </head>
 
<body>
  <h1>ESP32 Scale server</h1>
  <h3>WiFi client and Access Point mode (AP+STA)</h3>  
  
	<!--
	  
	  if(led1stat)
		{<p>Indicator state: ON</p><a class=\"button button-off\" href=\"/led1off\">Switch OFF</a>}
	  else
		{<p>Indicator state: OFF</p><a class=\"button button-on\" href=\"/led1on\">Switch ON  </a>}
	-->
 
  <h3>Weight:<br><span id="Weight">....</span></h3>

    
  <script>					// JS script Ajax
    
    setInterval(function() {
      getData();
      }, 2000);  				// every 2000 mS
    
    function getData() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("Weight").innerHTML = this.responseText;  // insert entire response into 'Weight' element. W/o parsing
        }
      };
      xhttp.open("GET", "/readWeight", true);
      xhttp.send();
    }
  </script>  
  
</body>
</html>
)=====";
