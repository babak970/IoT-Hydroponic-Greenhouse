#ifndef WEBPAGE_H
#define WEBPAGE_H

const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Hydroponic Greenhouse</title>
<style>
:root{
  --green:#19b77d;--green2:#08744f;--red:#ef4444;--amber:#f59e0b;
  --blue:#3b82f6;--ink:#13231d;--muted:#718078;--line:#e5ece8;
  --bg:#f3f8f5;--card:#fff;--shadow:0 10px 28px rgba(24,68,48,.08)
}
*{box-sizing:border-box}
body{margin:0;background:var(--bg);color:var(--ink);font-family:Arial,Helvetica,sans-serif}
.app{width:min(100%,980px);margin:auto;padding:16px}
.hero{padding:23px;border-radius:23px;color:#fff;background:linear-gradient(135deg,var(--green2),#23c58a);box-shadow:var(--shadow)}
.heroRow{display:flex;align-items:center;justify-content:space-between;gap:15px;flex-wrap:wrap}
h1{font-size:clamp(24px,5vw,38px);margin:0 0 5px}.sub{opacity:.83;font-size:13px}
.connected{display:flex;align-items:center;gap:8px;padding:9px 13px;border:1px solid #ffffff40;border-radius:99px;background:#ffffff1c;font-size:12px;font-weight:bold}
.dot{width:9px;height:9px;border-radius:50%;background:#74f2bc;box-shadow:0 0 0 5px #74f2bc25}
.title{margin:25px 2px 11px;font-size:18px}
.grid2,.devices,.timers{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:13px}
.card{background:var(--card);border:1px solid var(--line);border-radius:20px;box-shadow:var(--shadow)}
.gaugeCard{text-align:center;padding:19px}
.label{font-size:12px;color:var(--muted);font-weight:bold}
.gauge{width:145px;height:145px;position:relative;margin:8px auto 0}
.gauge svg{width:100%;height:100%;transform:rotate(-90deg)}
.track,.ring{fill:none;stroke-width:12}.track{stroke:#e7eeea}.ring{stroke-linecap:round;stroke-dasharray:314;transition:stroke-dashoffset .5s ease}.tempRing{stroke:var(--amber)}.humRing{stroke:var(--green)}
.center{position:absolute;inset:0;display:grid;place-content:center;font-size:29px;font-weight:800}.center small{display:block;font-size:11px;color:var(--muted);margin-top:4px}
.smallCards{display:grid;grid-template-columns:1fr 1fr;gap:13px;margin-top:13px}.smallCard{padding:18px}.big{font-size:23px;font-weight:800;margin-top:6px}
.chartCard{padding:17px;margin-top:13px}.chartHead{display:flex;justify-content:space-between;align-items:center;margin-bottom:8px}.chartLegend{font-size:11px;color:var(--muted)}
canvas{width:100%;height:115px;display:block}
.device{padding:17px}.deviceHead{display:flex;justify-content:space-between;gap:10px}.deviceName{font-size:17px;font-weight:800}.icon{font-size:27px}
.badges{display:flex;gap:7px;margin:13px 0;flex-wrap:wrap}.badge{font-size:10px;font-weight:800;padding:6px 10px;border-radius:99px;letter-spacing:.3px}
.on{background:#e4f8f0;color:#08724d}.off{background:#ffeded;color:#b42323}.auto{background:#eaf2ff;color:#245eaf}.manual{background:#fff4dc;color:#7c5615}
.controls{display:grid;grid-template-columns:1fr 1fr;gap:8px}.btn{border:0;border-radius:12px;padding:11px 9px;font-size:12px;font-weight:800;cursor:pointer}.btn:active{transform:scale(.98)}
.start{background:var(--green);color:#fff}.stop{background:var(--red);color:#fff}.neutral{background:#edf3ef;color:#244137}
.timerBox{padding:18px}.timerGroup{border:1px solid var(--line);padding:13px;border-radius:15px}.timerGroup h3{font-size:14px;margin:0 0 11px}
.inputs{display:grid;grid-template-columns:1fr 1fr;gap:9px}label{font-size:10px;color:var(--muted);font-weight:bold}
input{width:100%;margin-top:5px;border:1px solid #dce6df;border-radius:10px;background:#fbfdfc;padding:10px;font-size:14px;outline:none}
input:focus{border-color:var(--green);box-shadow:0 0 0 3px #19b77d20}.save{width:100%;margin-top:13px;padding:13px;border:0;border-radius:12px;background:linear-gradient(135deg,var(--green2),#20bd83);color:#fff;font-weight:800;cursor:pointer}
.toast{position:fixed;left:50%;bottom:22px;transform:translate(-50%,20px);padding:10px 15px;background:#173d2f;color:#fff;border-radius:99px;font-size:12px;opacity:0;pointer-events:none;transition:.25s}.toast.show{opacity:1;transform:translate(-50%,0)}
.footer{text-align:center;color:var(--muted);font-size:11px;padding:22px 0 7px}
@media(max-width:680px){.app{padding:11px}.grid2,.devices,.timers{grid-template-columns:1fr}.hero{border-radius:19px}.gauge{width:135px;height:135px}}
@media(max-width:390px){.smallCards,.controls,.inputs{grid-template-columns:1fr}}
</style>
</head>
<body>
<main class="app">
  <section class="hero">
    <div class="heroRow">
      <div><h1>Hydroponic Greenhouse</h1><div class="sub">Live monitoring and smart actuator control</div></div>
      <div class="connected"><span class="dot"></span><span id="connectionText">Connected · 192.168.4.1</span></div>
    </div>
  </section>

  <h2 class="title">Live environment</h2>
  <section class="grid2">
    <article class="card gaugeCard">
      <div class="label">AIR TEMPERATURE</div>
      <div class="gauge">
        <svg viewBox="0 0 120 120">
          <circle class="track" cx="60" cy="60" r="50"/>
          <circle id="tempRing" class="ring tempRing" cx="60" cy="60" r="50" style="stroke-dashoffset:314"/>
        </svg>
        <div class="center"><span id="temperature">--</span><small>°C · range 0–50</small></div>
      </div>
    </article>

    <article class="card gaugeCard">
      <div class="label">HUMIDITY</div>
      <div class="gauge">
        <svg viewBox="0 0 120 120">
          <circle class="track" cx="60" cy="60" r="50"/>
          <circle id="humidityRing" class="ring humRing" cx="60" cy="60" r="50" style="stroke-dashoffset:314"/>
        </svg>
        <div class="center"><span id="humidity">--</span><small>% relative humidity</small></div>
      </div>
    </article>
  </section>

  <section class="smallCards">
    <article class="card smallCard"><div class="label">ROOT / NTC TEMPERATURE</div><div id="ntc" class="big">-- °C</div></article>
    <article class="card smallCard"><div class="label">SYSTEM UPTIME</div><div id="uptime" class="big">00:00:00</div></article>
  </section>

  <article class="card chartCard">
    <div class="chartHead"><strong>Live sensor trend</strong><span class="chartLegend">Temperature and humidity · last 30 samples</span></div>
    <canvas id="chart" width="900" height="230"></canvas>
  </article>

  <h2 class="title">Device control</h2>
  <section class="devices" id="deviceGrid"></section>

  <h2 class="title">Automatic timing</h2>
  <section class="card timerBox">
    <form id="timerForm">
      <div class="timers">
        <div class="timerGroup"><h3>🫧 Air Pump</h3><div class="inputs"><label>ON seconds<input id="airOn" name="airOn" type="number" min="1"></label><label>OFF seconds<input id="airOff" name="airOff" type="number" min="1"></label></div></div>
        <div class="timerGroup"><h3>💧 Water Pump</h3><div class="inputs"><label>ON seconds<input id="waterOn" name="waterOn" type="number" min="1"></label><label>OFF seconds<input id="waterOff" name="waterOff" type="number" min="1"></label></div></div>
        <div class="timerGroup"><h3>💡 Plant LED</h3><div class="inputs"><label>ON seconds<input id="ledOn" name="ledOn" type="number" min="1"></label><label>OFF seconds<input id="ledOff" name="ledOff" type="number" min="1"></label></div></div>
        <div class="timerGroup"><h3>🌫️ Cold Steam</h3><div class="inputs"><label>ON seconds<input id="steamOn" name="steamOn" type="number" min="1"></label><label>OFF seconds<input id="steamOff" name="steamOff" type="number" min="1"></label></div></div>
      </div>
      <button class="save" type="submit">Save all timer settings</button>
    </form>
  </section>

  <div class="footer">Greenhouse_AP · Values update without reloading the page</div>
</main>
<div id="toast" class="toast">Saved</div>

<script>
const devices=[
  {key:"air",name:"Air Pump",sub:"Oxygen circulation",icon:"🫧",timed:true},
  {key:"water",name:"Water Pump",sub:"Irrigation circulation",icon:"💧",timed:true},
  {key:"led",name:"Plant LED",sub:"Grow-light cycle",icon:"💡",timed:true},
  {key:"steam",name:"Cold Steam",sub:"Humidity support",icon:"🌫️",timed:true},
  {key:"heater",name:"Heater",sub:"Manual temperature support",icon:"🔥",timed:false}
];

let firstData=true;
let busy=false;
const tempHistory=[],humHistory=[];

function deviceMarkup(d){
  return `<article class="card device">
    <div class="deviceHead"><div><div class="deviceName">${d.name}</div><div class="sub">${d.sub}</div></div><div class="icon">${d.icon}</div></div>
    <div class="badges"><span id="${d.key}State" class="badge off">OFF</span><span id="${d.key}Mode" class="badge manual">${d.timed?"AUTO":"MANUAL"}</span></div>
    <div class="controls">
      <button id="${d.key}Toggle" class="btn start" onclick="toggleDevice('${d.key}')">Turn ON</button>
      ${d.timed?`<button class="btn neutral" onclick="toggleMode('${d.key}')">Change mode</button>`:""}
    </div>
  </article>`;
}
document.getElementById("deviceGrid").innerHTML=devices.map(deviceMarkup).join("");

function showToast(text){
  const t=document.getElementById("toast");
  t.textContent=text;t.classList.add("show");
  setTimeout(()=>t.classList.remove("show"),1500);
}

function formatUptime(total){
  const days=Math.floor(total/86400);
  const h=Math.floor(total%86400/3600);
  const m=Math.floor(total%3600/60);
  const s=total%60;
  const clock=[h,m,s].map(v=>String(v).padStart(2,"0")).join(":");
  return days?days+"d "+clock:clock;
}

function setGauge(id,value,max){
  const safe=value==null?0:Math.max(0,Math.min(max,value));
  document.getElementById(id).style.strokeDashoffset=314*(1-safe/max);
}

function updateDevice(key,data){
  const state=document.getElementById(key+"State");
  const mode=document.getElementById(key+"Mode");
  const button=document.getElementById(key+"Toggle");

  state.textContent=data.state?"ON":"OFF";
  state.className="badge "+(data.state?"on":"off");

  if(mode){
    mode.textContent=data.auto===undefined?"MANUAL":(data.auto?"AUTO":"MANUAL");
    mode.className="badge "+(data.auto?"auto":"manual");
  }

  button.textContent=data.state?"Turn OFF":"Turn ON";
  button.className="btn "+(data.state?"stop":"start");
}

function updateInputs(data){
  const values={
    airOn:data.air.on,airOff:data.air.off,
    waterOn:data.water.on,waterOff:data.water.off,
    ledOn:data.led.on,ledOff:data.led.off,
    steamOn:data.steam.on,steamOff:data.steam.off
  };
  for(const id in values){
    const input=document.getElementById(id);
    if(firstData||document.activeElement!==input) input.value=values[id];
  }
}

function drawChart(){
  const canvas=document.getElementById("chart");
  const ctx=canvas.getContext("2d");
  const w=canvas.width,h=canvas.height,p=28;
  ctx.clearRect(0,0,w,h);
  ctx.strokeStyle="#e4ebe7";ctx.lineWidth=1;
  for(let i=0;i<4;i++){const y=p+i*(h-2*p)/3;ctx.beginPath();ctx.moveTo(p,y);ctx.lineTo(w-p,y);ctx.stroke()}

  function line(values,min,max,color){
    if(values.length<2)return;
    ctx.strokeStyle=color;ctx.lineWidth=4;ctx.lineJoin="round";ctx.lineCap="round";ctx.beginPath();
    values.forEach((v,i)=>{
      const x=p+i*(w-2*p)/(Math.max(29,values.length-1));
      const y=h-p-(v-min)/(max-min)*(h-2*p);
      i?ctx.lineTo(x,y):ctx.moveTo(x,y);
    });
    ctx.stroke();
  }
  line(tempHistory,0,50,"#f59e0b");
  line(humHistory,0,100,"#19b77d");
}

async function fetchStatus(){
  if(busy)return;
  try{
    const r=await fetch("/api/status",{cache:"no-store"});
    if(!r.ok)throw new Error();
    const d=await r.json();

    document.getElementById("connectionText").textContent="Connected · 192.168.4.1";
    document.getElementById("temperature").textContent=d.temperature==null?"--":d.temperature.toFixed(1);
    document.getElementById("humidity").textContent=d.humidity==null?"--":Math.round(d.humidity);
    document.getElementById("ntc").textContent=d.ntc==null?"Sensor error":d.ntc.toFixed(1)+" °C";
    document.getElementById("uptime").textContent=formatUptime(d.uptime);

    setGauge("tempRing",d.temperature,50);
    setGauge("humidityRing",d.humidity,100);

    updateDevice("air",d.air);updateDevice("water",d.water);
    updateDevice("led",d.led);updateDevice("steam",d.steam);
    updateDevice("heater",d.heater);updateInputs(d);

    if(d.temperature!=null){tempHistory.push(d.temperature);if(tempHistory.length>30)tempHistory.shift()}
    if(d.humidity!=null){humHistory.push(d.humidity);if(humHistory.length>30)humHistory.shift()}
    drawChart();
    firstData=false;
  }catch(e){
    document.getElementById("connectionText").textContent="Reconnecting...";
  }
}

async function command(url){
  busy=true;
  try{
    const r=await fetch(url,{cache:"no-store"});
    if(!r.ok)throw new Error();
    await fetchStatus();
  }catch(e){showToast("Command failed")}
  busy=false;
  fetchStatus();
}

function toggleDevice(key){command("/api/toggle?device="+encodeURIComponent(key))}
function toggleMode(key){command("/api/mode?device="+encodeURIComponent(key))}

document.getElementById("timerForm").addEventListener("submit",async e=>{
  e.preventDefault();
  const q=new URLSearchParams(new FormData(e.target));
  busy=true;
  try{
    const r=await fetch("/api/timers?"+q.toString(),{cache:"no-store"});
    if(!r.ok)throw new Error();
    showToast("Timer settings saved");
  }catch(err){showToast("Save failed")}
  busy=false;
  fetchStatus();
});

fetchStatus();
setInterval(fetchStatus,2000);
</script>
</body>
</html>
)rawliteral";

#endif
