//helper funs

const d = i => document.getElementById(i);
const ae = (o, e, f) => { o.addEventListener(e, f) };

//global variables
//error messages handler
var ermid = null;
const HOST = window.location.origin;
const TIMEOUT_DELAY = 5e3;
//validate credentials
const vdt = (sid, psd) => {
    if (sid.length === 0 || psd.length === 0) return false;
    if (!sid.match(/^[^!#;+\]\/"\t][^+\]"\t]{0,31}$/)) return false;
    if (!psd.match(/^[\u0020-\u007e]{8,63}$/)) return false;
    return true;
}
//display error for 3s
const rerr = () => {
    const id = 'ID_ERR_MSG';
    const bid = 'ID_ERR_BG';
    const p_ = d(id);
    const b_ = d(bid);
    p_.innerText = "";
    b_.classList.remove('err_m');
    b_.classList.add("d-none");
    ermid = null;
}
const derr = (m) => {
    const id = 'ID_ERR_MSG';
    const bid = 'ID_ERR_BG';
    const p_ = d(id);
    const b_ = d(bid);
    //add error
    p_.innerText = m;
    if (ermid === null) {
        b_.classList.remove('d-none');
        b_.classList.add('err_m');
        ermid = setTimeout(rerr, TIMEOUT_DELAY);
        return;
    }
    clearTimeout(ermid);
    ermid = setTimeout(rerr, TIMEOUT_DELAY);
}

//send credentials via POST

const __pf = (s_, p_) => fetch(`${HOST}/creds`, {
    method: 'POST',
    headers: {
        'content-type': 'application/json'
    },
    body: JSON.stringify(
        {
            ssid: s_,
            pass: p_
        }
    )
}).then(res => {
    if (!res.ok) {
        console.log(`HTTP error! Status : ${res.status}`);
        derr(`HTTP error! Status : ${res.status}`);
    }
    return res.text();
}).then(data => console.log(data)
).catch(e => {
    console.log(e);
    derr(e);
})

//submit credentials (e)
const s_creds = () => {
    const ssid_ = `ID_AP_SSID`;
    const pass_ = `ID_AP_PASS`;
    const s_t = d(ssid_);
    const p_t = d(pass_)
    const ssid = s_t.value;
    const pass = p_t.value;
    s_t.value = "";
    p_t.value = "";
    if (!vdt(ssid, pass)) {
        derr(`The AP credentials are invalid`);
        return;
    }
    console.log(ssid, pass);
    __pf(ssid, pass);
}
const s_cmd = (cmd, p_, v_) => fetch(v_ === undefined ? `${HOST}/${cmd}` : `${HOST}/${cmd}?${p_}=${v_}`)
    .then((res) => {
        if (!res.ok) {
            console.log(`HTTP error! Status : ${res.status}`);
            derr(`HTTP error! Status : ${res.status}`);
        }
        return res.text();
    })
    .then(data => console.log(data))
    .catch(e => {
        console.log(e);
        derr(e);
    });



const initialize = () => {
    const theme = localStorage.getItem('theme');
    const c_dark = theme === 'dark' ? 'd-none' : 'top-svg';
    const c_light = theme === 'dark' ? 'top-svg' : 'd-none';
    d('ID_DARK_IMG').setAttribute('class', c_dark);
    d('ID_LIGHT_IMG').setAttribute('class', c_light);
    const colorTheme = theme === 'dark' ? 'dark' : 'light';
    document.documentElement.setAttribute('data-theme', colorTheme);
    fetch(`${HOST}/init`)
    .then((res) => {
        if (!res.ok) {
            console.log(`HTTP error! Status : ${res.status}`);
            derr(`Error in getting init data : ${res.status}`);
        }
        return res.json();
    })
    .then(data =>{
        console.log(data);
        updateColor(data.color);
        d('ID_BRIGHT').value = data.bright;
        d('ID_BRIGHT_VALUE').innerText = data.bright;
        d('ID_POWER').checked = data.power === 1;
    })
    .catch(e => {
        console.log(e);
        derr(e);
    });
}

const toggleTheme = (e) => { 
    let _id = e.target.id.substring(3);
    console.log({ _id });
    _id = _id.substring(0, _id.indexOf('_')).toLowerCase();
    const colorTheme = localStorage.getItem('theme') === 'dark';
    const theme = colorTheme ? 'light' : 'dark';
    localStorage.setItem('theme', theme);
    document.documentElement.setAttribute('data-theme', theme);
    const c_dark = theme === 'dark' ? 'd-none' : 'top-svg';
    const c_light = theme === 'dark' ? 'top-svg' : 'd-none';
    d('ID_DARK_IMG').setAttribute('class', c_dark);
    d('ID_LIGHT_IMG').setAttribute('class', c_light);
}

const toggleSettings = () => {
    const isSettings = d('ID_CONTENT').getAttribute('class') === 'd-none';
    const content = isSettings? 'content' : 'd-none';
    const settings = isSettings? 'd-none' : 'content';
    d('ID_CONTENT').setAttribute('class', content);
    d('ID_SETTINGS').setAttribute('class', settings);
}

const updateBrightness = (e) => {
    d('ID_BRIGHT_VALUE').innerText = e.target.value;
}

const hfileChange = (e) => {
    const file = e.target.files[0];
    const btn = d('ID_F_BTN');
    const fil = d('ID_F_UPDATE_LABEL');
    const ext = file.name.substring(file.name.lastIndexOf('.'));
    if (ext !== '.bin') {
        derr("Invalid file format");
        btn.disabled = true;
        file.value = '';
        return;
    }
    const fn = file.name.length > 15 ? `${file.name.substring(0, 15)}...` : file.name;
    fil.innerText = fn;
    btn.disabled = false;
}

const hFileUpload = () => {
    const fileInp = d('ID_UPDATE_FILE');
    const file = fileInp.files[0];
    const formData = new FormData();
    const progressBar = d('ID_F_PROGRESS');
    formData.append('file', file);

    xhr = new XMLHttpRequest();
    xhr.open('POST', `${HOST}/update`, true);

    xhr.upload.onprogress = (e) => {
        if (e.lengthComputable) {
            const progress = (e.loaded / e.total) * 100;
            progressBar.style.width = `${progress}%`;
            if(progress === 100){
                derr("File Uploaded");
                xhr = null;
            }
        }
    };
    xhr.onreadystatechange = () => {
        if(xhr.readyState === 4 && xhr.status === 200){
            console.log(xhr.responseText);
            derr(xhr.responseText);
        }
        xhr = null;
    }
    

    xhr.send(formData);
}

const updateColor = (color) =>{
    const dColor = d('ID_S_COLOR');
    dColor.style.backgroundColor = color;
    dColor.value = color;
}
//ready
ae(document, 'DOMContentLoaded', () => {
    initialize();
    ae(d('ID_AP_SUBMIT'), 'click', s_creds);
    ae(d('ID_AP_RESET'), 'click', () => s_cmd('reset'));
    ae(d('ID_ALL_RESET'), 'click', () => s_cmd('rmrf'));
    ae(d('ID_BRIGHT'), 'input', updateBrightness);
    ae(d('ID_THEME_BTN'), 'click', (e) => toggleTheme(e));
    ae(d('ID_SETTINGS_BTN'), 'click', toggleSettings);

    ae(d('ID_POWER'), 'change', (e)=>s_cmd('power', 'value', e.target.checked? 'y':'n'));
    ae(d('ID_S_COLOR'), 'input', (e)=>updateColor(e.target.value));
    ae(d('ID_S_COLOR'), 'change', (e)=>s_cmd('color', 'value', e.target.value.substring(1)));

    ae(d('ID_BRIGHT'), 'change', (e) => s_cmd('bright', 'value', e.target.value));
    ae(d('ID_ERR_MSG_CLOSE'), 'click', (e) => {
        if (ermid) {
            clearTimeout(ermid);
            rerr();
        }
    });
    ae(d('ID_UPDATE_FILE'), 'change', hfileChange);
    ae(d('ID_F_BTN'), 'click', hFileUpload);

});






