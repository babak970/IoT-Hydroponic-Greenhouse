#ifndef WEBPAGE_H
#define WEBPAGE_H

const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="refresh" content="5">
  <title>Hydroponic Greenhouse</title>

  <style>
    :root {
      --green: #1db980;
      --green-dark: #0d7c57;
      --green-soft: #e9f8f2;
      --red: #ef4444;
      --red-soft: #fff0f0;
      --amber: #f59e0b;
      --blue: #3b82f6;
      --ink: #13231d;
      --muted: #718078;
      --line: #e6ece8;
      --surface: #ffffff;
      --background: #f4f8f6;
      --shadow: 0 10px 30px rgba(26, 61, 45, 0.08);
    }

    * {
      box-sizing: border-box;
    }

    body {
      margin: 0;
      background: var(--background);
      color: var(--ink);
      font-family: Arial, Helvetica, sans-serif;
    }

    .app {
      width: min(100%, 980px);
      margin: 0 auto;
      padding: 18px;
    }

    .topbar {
      background: linear-gradient(135deg, #0b6c4b, #22bd83);
      color: white;
      border-radius: 24px;
      padding: 24px;
      box-shadow: var(--shadow);
      margin-bottom: 18px;
    }

    .topbar-row {
      display: flex;
      justify-content: space-between;
      align-items: center;
      gap: 16px;
      flex-wrap: wrap;
    }

    h1 {
      margin: 0 0 6px;
      font-size: clamp(24px, 5vw, 38px);
    }

    .subtitle {
      margin: 0;
      opacity: 0.83;
      font-size: 14px;
    }

    .connection {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      padding: 9px 13px;
      border-radius: 999px;
      background: rgba(255,255,255,0.15);
      border: 1px solid rgba(255,255,255,0.22);
      font-size: 13px;
      font-weight: 700;
    }

    .dot {
      width: 9px;
      height: 9px;
      border-radius: 50%;
      background: #74f2bc;
      box-shadow: 0 0 0 5px rgba(116,242,188,0.15);
    }

    .section-title {
      margin: 26px 2px 12px;
      font-size: 18px;
    }

    .sensor-grid {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 14px;
    }

    .gauge-card,
    .info-card,
    .device-card,
    .timer-card {
      background: var(--surface);
      border: 1px solid var(--line);
      border-radius: 20px;
      box-shadow: var(--shadow);
    }

    .gauge-card {
      padding: 20px;
      text-align: center;
    }

    .gauge-title {
      color: var(--muted);
      font-size: 13px;
      font-weight: 700;
      margin-bottom: 8px;
    }

    .gauge {
      width: 148px;
      height: 148px;
      margin: 0 auto;
      position: relative;
    }

    .gauge svg {
      width: 100%;
      height: 100%;
      transform: rotate(-90deg);
    }

    .track {
      fill: none;
      stroke: #e7eeea;
      stroke-width: 12;
    }

    .progress {
      fill: none;
      stroke-width: 12;
      stroke-linecap: round;
      stroke-dasharray: 314;
      transition: stroke-dashoffset .5s ease;
    }

    .temperature {
      stroke: var(--amber);
    }

    .humidity {
      stroke: var(--green);
    }

    .gauge-value {
      position: absolute;
      inset: 0;
      display: grid;
      place-content: center;
      font-size: 29px;
      font-weight: 800;
    }

    .gauge-value small {
      display: block;
      color: var(--muted);
      font-size: 12px;
      font-weight: 600;
      margin-top: 4px;
    }

    .mini-grid {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 14px;
      margin-top: 14px;
    }

    .info-card {
      padding: 18px;
    }

    .info-label {
      color: var(--muted);
      font-size: 12px;
      font-weight: 700;
      margin-bottom: 7px;
    }

    .info-value {
      font-size: 24px;
      font-weight: 800;
    }

    .device-grid {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 14px;
    }

    .device-card {
      padding: 18px;
    }

    .device-head {
      display: flex;
      justify-content: space-between;
      align-items: flex-start;
      gap: 12px;
    }

    .device-name {
      font-size: 17px;
      font-weight: 800;
      margin: 0 0 5px;
    }

    .device-icon {
      font-size: 28px;
    }

    .badges {
      display: flex;
      gap: 7px;
      flex-wrap: wrap;
      margin: 14px 0;
    }

    .badge {
      border-radius: 999px;
      padding: 6px 10px;
      font-size: 11px;
      font-weight: 800;
      letter-spacing: .3px;
    }

    .status-on {
      color: #08724d;
      background: #e6f8f1;
    }

    .status-off {
      color: #b42323;
      background: #ffeded;
    }

    .mode-auto {
      color: #245eaf;
      background: #eaf2ff;
    }

    .mode-manual {
      color: #7c5615;
      background: #fff5dc;
    }

    .control-row {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 9px;
    }

    .btn {
      display: block;
      text-align: center;
      text-decoration: none;
      border-radius: 12px;
      padding: 11px 10px;
      font-size: 13px;
      font-weight: 800;
      transition: transform .15s ease;
    }

    .btn:active {
      transform: scale(.98);
    }

    .success {
      background: var(--green);
      color: white;
    }

    .danger {
      background: var(--red);
      color: white;
    }

    .secondary {
      background: #edf3ef;
      color: #244137;
    }

    .timer-card {
      padding: 20px;
    }

    .timer-grid {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 14px;
    }

    .timer-group {
      border: 1px solid var(--line);
      border-radius: 16px;
      padding: 14px;
    }

    .timer-group h3 {
      margin: 0 0 12px;
      font-size: 15px;
    }

    .input-row {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 10px;
    }

    label {
      display: block;
      color: var(--muted);
      font-size: 11px;
      font-weight: 700;
    }

    input {
      width: 100%;
      margin-top: 5px;
      border: 1px solid #dce6df;
      border-radius: 11px;
      padding: 11px;
      font-size: 15px;
      color: var(--ink);
      background: #fbfdfc;
      outline: none;
    }

    input:focus {
      border-color: var(--green);
      box-shadow: 0 0 0 3px rgba(29,185,128,.12);
    }

    .save {
      border: 0;
      width: 100%;
      margin-top: 14px;
      padding: 13px;
      border-radius: 13px;
      background: linear-gradient(135deg, #0f815b, #20bd83);
      color: white;
      font-size: 14px;
      font-weight: 800;
      cursor: pointer;
    }

    .footer {
      text-align: center;
      color: var(--muted);
      font-size: 12px;
      padding: 22px 0 8px;
    }

    @media (max-width: 680px) {
      .app {
        padding: 12px;
      }

      .topbar {
        border-radius: 19px;
        padding: 20px;
      }

      .sensor-grid,
      .device-grid,
      .timer-grid {
        grid-template-columns: 1fr;
      }

      .mini-grid {
        grid-template-columns: 1fr 1fr;
      }
    }

    @media (max-width: 390px) {
      .mini-grid,
      .control-row,
      .input-row {
        grid-template-columns: 1fr;
      }
    }
  </style>
</head>

<body>
  <main class="app">
    <section class="topbar">
      <div class="topbar-row">
        <div>
          <h1>Hydroponic Greenhouse</h1>
          <p class="subtitle">Smart monitoring and actuator control</p>
        </div>

        <div class="connection">
          <span class="dot"></span>
          Connected · 192.168.4.1
        </div>
      </div>
    </section>

    <h2 class="section-title">Live environment</h2>

    <section class="sensor-grid">
      <article class="gauge-card">
        <div class="gauge-title">Air temperature</div>
        <div class="gauge">
          <svg viewBox="0 0 120 120">
            <circle class="track" cx="60" cy="60" r="50"></circle>
            <circle class="progress temperature" cx="60" cy="60" r="50"
                    style="stroke-dashoffset:%TEMP_OFFSET%"></circle>
          </svg>
          <div class="gauge-value">
            %AIR_TEMP%°C
            <small>Range 0–50°C</small>
          </div>
        </div>
      </article>

      <article class="gauge-card">
        <div class="gauge-title">Humidity</div>
        <div class="gauge">
          <svg viewBox="0 0 120 120">
            <circle class="track" cx="60" cy="60" r="50"></circle>
            <circle class="progress humidity" cx="60" cy="60" r="50"
                    style="stroke-dashoffset:%HUM_OFFSET%"></circle>
          </svg>
          <div class="gauge-value">
            %HUMIDITY%%
            <small>Relative humidity</small>
          </div>
        </div>
      </article>
    </section>

    <section class="mini-grid">
      <article class="info-card">
        <div class="info-label">ROOT / NTC TEMPERATURE</div>
        <div class="info-value">%NTC_TEMP%°C</div>
      </article>

      <article class="info-card">
        <div class="info-label">SYSTEM UPTIME</div>
        <div class="info-value">%UPTIME%</div>
      </article>
    </section>

    <h2 class="section-title">Device control</h2>

    <section class="device-grid">
      <article class="device-card">
        <div class="device-head">
          <div>
            <h3 class="device-name">Air Pump</h3>
            <div class="subtitle">Oxygen circulation</div>
          </div>
          <div class="device-icon">🫧</div>
        </div>

        <div class="badges">
          <span class="badge %AIR_STATE_CLASS%">%AIR_STATE%</span>
          <span class="badge %AIR_MODE_CLASS%">%AIR_MODE%</span>
        </div>

        <div class="control-row">
          <a class="btn %AIR_ACTION_CLASS%" href="/air/toggle">%AIR_ACTION%</a>
          <a class="btn secondary" href="/air/mode">Change mode</a>
        </div>
      </article>

      <article class="device-card">
        <div class="device-head">
          <div>
            <h3 class="device-name">Water Pump</h3>
            <div class="subtitle">Irrigation circulation</div>
          </div>
          <div class="device-icon">💧</div>
        </div>

        <div class="badges">
          <span class="badge %WATER_STATE_CLASS%">%WATER_STATE%</span>
          <span class="badge %WATER_MODE_CLASS%">%WATER_MODE%</span>
        </div>

        <div class="control-row">
          <a class="btn %WATER_ACTION_CLASS%" href="/water/toggle">%WATER_ACTION%</a>
          <a class="btn secondary" href="/water/mode">Change mode</a>
        </div>
      </article>

      <article class="device-card">
        <div class="device-head">
          <div>
            <h3 class="device-name">Plant LED</h3>
            <div class="subtitle">Grow-light cycle</div>
          </div>
          <div class="device-icon">💡</div>
        </div>

        <div class="badges">
          <span class="badge %LED_STATE_CLASS%">%LED_STATE%</span>
          <span class="badge %LED_MODE_CLASS%">%LED_MODE%</span>
        </div>

        <div class="control-row">
          <a class="btn %LED_ACTION_CLASS%" href="/led/toggle">%LED_ACTION%</a>
          <a class="btn secondary" href="/led/mode">Change mode</a>
        </div>
      </article>

      <article class="device-card">
        <div class="device-head">
          <div>
            <h3 class="device-name">Cold Steam</h3>
            <div class="subtitle">Humidity support</div>
          </div>
          <div class="device-icon">🌫️</div>
        </div>

        <div class="badges">
          <span class="badge %STEAM_STATE_CLASS%">%STEAM_STATE%</span>
          <span class="badge %STEAM_MODE_CLASS%">%STEAM_MODE%</span>
        </div>

        <div class="control-row">
          <a class="btn %STEAM_ACTION_CLASS%" href="/steam/toggle">%STEAM_ACTION%</a>
          <a class="btn secondary" href="/steam/mode">Change mode</a>
        </div>
      </article>

      <article class="device-card">
        <div class="device-head">
          <div>
            <h3 class="device-name">Heater</h3>
            <div class="subtitle">Manual control</div>
          </div>
          <div class="device-icon">🔥</div>
        </div>

        <div class="badges">
          <span class="badge %HEATER_STATE_CLASS%">%HEATER_STATE%</span>
          <span class="badge mode-manual">MANUAL</span>
        </div>

        <div class="control-row" style="grid-template-columns:1fr">
          <a class="btn %HEATER_ACTION_CLASS%" href="/heater/toggle">%HEATER_ACTION%</a>
        </div>
      </article>
    </section>

    <h2 class="section-title">Automatic timing</h2>

    <section class="timer-card">
      <form action="/set" method="GET">
        <div class="timer-grid">
          <div class="timer-group">
            <h3>🫧 Air Pump</h3>
            <div class="input-row">
              <label>
                ON seconds
                <input name="airOn" type="number" min="1" value="%AIR_ON%">
              </label>
              <label>
                OFF seconds
                <input name="airOff" type="number" min="1" value="%AIR_OFF%">
              </label>
            </div>
          </div>

          <div class="timer-group">
            <h3>💧 Water Pump</h3>
            <div class="input-row">
              <label>
                ON seconds
                <input name="waterOn" type="number" min="1" value="%WATER_ON%">
              </label>
              <label>
                OFF seconds
                <input name="waterOff" type="number" min="1" value="%WATER_OFF%">
              </label>
            </div>
          </div>

          <div class="timer-group">
            <h3>💡 Plant LED</h3>
            <div class="input-row">
              <label>
                ON seconds
                <input name="ledOn" type="number" min="1" value="%LED_ON%">
              </label>
              <label>
                OFF seconds
                <input name="ledOff" type="number" min="1" value="%LED_OFF%">
              </label>
            </div>
          </div>

          <div class="timer-group">
            <h3>🌫️ Cold Steam</h3>
            <div class="input-row">
              <label>
                ON seconds
                <input name="steamOn" type="number" min="1" value="%STEAM_ON%">
              </label>
              <label>
                OFF seconds
                <input name="steamOff" type="number" min="1" value="%STEAM_OFF%">
              </label>
            </div>
          </div>
        </div>

        <button class="save" type="submit">Save all timer settings</button>
      </form>
    </section>

    <div class="footer">
      Greenhouse_AP · Dashboard refreshes every 5 seconds
    </div>
  </main>
</body>
</html>
)rawliteral";

#endif
