const char _index[] PROGMEM = R"=====(
<!doctype html>
<html lang=en>
<head>
<meta charset=UTF-8>
<meta http-equiv=X-UA-Compatible content="IE=edge">
<meta name=viewport content="width=device-width,initial-scale=1">
<link rel=stylesheet href=styles.css>
<title>ESP8266FSAutoConnect</title>
</head>
<body>
<div class=container>
<div class=topbar>
<p class=title>Control Panel</p>
<button id=ID_THEME_BTN class=top-svg-btn>
<svg xmlns=http://www.w3.org/2000/svg id=ID_DARK_IMG class=top-svg viewBox="0 0 24 24" fill=none stroke-width=2 stroke-linecap=round stroke-linejoin=round class="feather feather-moon">
<path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"></path>
</svg>
<svg xmlns=http://www.w3.org/2000/svg id=ID_LIGHT_IMG class=d-none viewBox="0 0 24 24" fill=none stroke-width=2 stroke-linecap=round stroke-linejoin=round class="feather feather-sun">
<circle cx=12 cy=12 r=5></circle>
<line x1=12 y1=1 x2=12 y2=3></line>
<line x1=12 y1=21 x2=12 y2=23></line>
<line x1=4.22 y1=4.22 x2=5.64 y2=5.64></line>
<line x1=18.36 y1=18.36 x2=19.78 y2=19.78></line>
<line x1=1 y1=12 x2=3 y2=12></line>
<line x1=21 y1=12 x2=23 y2=12></line>
<line x1=4.22 y1=19.78 x2=5.64 y2=18.36></line>
<line x1=18.36 y1=5.64 x2=19.78 y2=4.22></line>
</svg>
</button>
<button id=ID_SETTINGS_BTN class=top-svg-btn>
<svg xmlns=http://www.w3.org/2000/svg class=top-svg viewBox="0 0 24 24" fill=none stroke-width=2 stroke-linecap=round stroke-linejoin=round class="feather feather-settings">
<circle cx=12 cy=12 r=3></circle>
<path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z">
</path>
</svg>
</button>
</div>
<div class=d-none id=ID_ERR_BG>
<p id=ID_ERR_MSG></p>
<button id=ID_ERR_MSG_CLOSE class=close-btn>
<svg xmlns=http://www.w3.org/2000/svg viewBox="0 0 24 24" fill=none class=close-btn-svg stroke-width=2 stroke-linecap=round stroke-linejoin=round class="feather feather-x">
<line x1=18 y1=6 x2=6 y2=18></line>
<line x1=6 y1=6 x2=18 y2=18></line>
</svg>
</button>
</div>
<div id=ID_CONTENT class=content>
<div class=card>
<p class=card-title>Choose Color</p>
<div class=card-row>
<label class=switch>
<input type=checkbox id=ID_POWER>
<span class=check></span>
</label>
<input class=color-picker id=ID_S_COLOR type=color value=#000000>
<input id=ID_BRIGHT class=slider type=range min=0 max=100 value=0>
<div class=angle-wrap>
<p id=ID_BRIGHT_VALUE class=slider-value>-</p>
</div>
</div>
</div>
</div>
<div id=ID_SETTINGS class=d-none>
<div class=card>
<p class=card-title>AP Credentials</p>
<div class=card-row>
<input class=card-textbox id=ID_AP_SSID placeholder="Enter SSID">
<input type=password class=card-textbox id=ID_AP_PASS placeholder="Enter Password">
</div>
<div class=card-row>
<button id=ID_AP_SUBMIT class=btn>Submit</button>
</div>
</div>
<div class=card>
<p class=card-title>Settings</p>
<div class=card-row>
<button id=ID_AP_RESET class=btn>Reset AP Credentials</button>
<button id=ID_ALL_RESET class=btn>Factory Reset</button>
</div>
</div>
<div class=card>
<div class=card-title>OTA Firmware Update</div>
<div class=card-row>
<div class=filebox-wrapper>
<div id=ID_F_PROGRESS class=progress-bar></div>
<div class=filebox-combo>
<input class=filebox type=file id=ID_UPDATE_FILE name=update>
<label id=ID_F_UPDATE_LABEL for=ID_UPDATE_FILE>Select File</label>
<button id=ID_F_BTN disabled>
<svg xmlns=http://www.w3.org/2000/svg viewBox="0 0 24 24" fill=none stroke-width=2 stroke-linecap=round stroke-linejoin=round class="feather feather-arrow-right">
<line x1=5 y1=12 x2=19 y2=12></line>
<polyline points="12 5 19 12 12 19"></polyline>
</svg>
</button>
</div>
</div>
</div>
</div>
</div>
</div>
<script src=script.js></script>
</body>
</html>
)=====";