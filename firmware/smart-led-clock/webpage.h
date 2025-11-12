/**
 * @file webpage.h
 * @brief HTML pages for web interface
 * 
 * Contains all HTML pages stored in PROGMEM to save RAM.
 * Separated from webserver.cpp to keep code organized.
 * 
 * Pages included:
 * - WEBPAGE_HOME: Main dashboard with sensor data
 * - WEBPAGE_CONFIG: Configuration page
 * 
 * @author F. Baillon
 * @version 1.1.0
 * @date January 2025
 * @license MIT License
 */

#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Arduino.h>

// ==========================================
// COMMON HTML COMPONENTS
// ==========================================

// Common CSS styles used across pages
const char HTML_STYLE_COMMON[] PROGMEM = R"rawliteral(
body {
    font-family: Arial, sans-serif;
    max-width: 800px;
    margin: 50px auto;
    padding: 20px;
    background: #f0f0f0;
}
.container {
    background: white;
    padding: 30px;
    border-radius: 10px;
    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
}
h1 {
    color: #333;
    text-align: center;
}
.nav {
    text-align: center;
    margin: 20px 0;
}
.nav a {
    color: #4CAF50;
    text-decoration: none;
    font-size: 18px;
    margin: 0 10px;
}
.nav a:hover {
    text-decoration: underline;
}
)rawliteral";

// ==========================================
// HOME PAGE (Dashboard)
// ==========================================

const char WEBPAGE_HOME[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart LED Clock</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
            background: #f0f0f0;
        }
        .container {
            background: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            text-align: center;
        }
        .nav {
            text-align: center;
            margin: 20px 0;
        }
        .nav a {
            color: #4CAF50;
            text-decoration: none;
            font-size: 18px;
        }
        .nav a:hover {
            text-decoration: underline;
        }
        .sensor-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
            margin-top: 30px;
        }
        .sensor-card {
            background: #f9f9f9;
            padding: 20px;
            border-radius: 8px;
            border-left: 4px solid #4CAF50;
        }
        .sensor-label {
            font-size: 14px;
            color: #666;
            margin-bottom: 5px;
        }
        .sensor-value {
            font-size: 28px;
            font-weight: bold;
            color: #333;
        }
        .sensor-unit {
            font-size: 18px;
            color: #999;
        }
        .update-time {
            text-align: center;
            color: #999;
            margin-top: 20px;
        }
        .refresh-btn {
            display: block;
            width: 200px;
            margin: 20px auto;
            padding: 12px;
            background: #4CAF50;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 16px;
        }
        .refresh-btn:hover {
            background: #45a049;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🕐 Smart LED Clock</h1>
        
        <div class="nav">
            <a href="/config">⚙️ Configuration</a> | 
            <a href="/moon">🌙 Phase Lunaire</a>
        </div>
        
        <div class="sensor-grid">
            <div class="sensor-card">
                <div class="sensor-label">Température Intérieure</div>
                <div class="sensor-value" id="tempIn">--</div>
                <div class="sensor-unit">°C</div>
            </div>
            
            <div class="sensor-card">
                <div class="sensor-label">Humidité Intérieure</div>
                <div class="sensor-value" id="humIn">--</div>
                <div class="sensor-unit">%</div>
            </div>
            
            <div class="sensor-card">
                <div class="sensor-label">Température Extérieure</div>
                <div class="sensor-value" id="tempOut">--</div>
                <div class="sensor-unit">°C</div>
            </div>
            
            <div class="sensor-card">
                <div class="sensor-label">Humidité Extérieure</div>
                <div class="sensor-value" id="humOut">--</div>
                <div class="sensor-unit">%</div>
            </div>
            
            <div class="sensor-card">
                <div class="sensor-label">Qualité de l'Air (AQI)</div>
                <div class="sensor-value" id="aqi">--</div>
                <div class="sensor-unit" id="aqiQuality">--</div>
            </div>
            
            <div class="sensor-card">
                <div class="sensor-label">Heure</div>
                <div class="sensor-value" id="time" style="font-size: 20px;">--:--:--</div>
            </div>
        </div>
        
        <button class="refresh-btn" onclick="updateData()">🔄 Rafraîchir</button>
        
        <div class="update-time">
            Dernière mise à jour: <span id="lastUpdate">--</span>
        </div>
    </div>
    
    <script>
        function updateData() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('tempIn').textContent = data.indoor.temp.toFixed(1);
                    document.getElementById('humIn').textContent = Math.round(data.indoor.humidity);
                    document.getElementById('tempOut').textContent = data.outdoor.temp.toFixed(1);
                    document.getElementById('humOut').textContent = Math.round(data.outdoor.humidity);
                    document.getElementById('aqi').textContent = data.airQuality.aqi;
                    document.getElementById('aqiQuality').textContent = data.airQuality.quality;
                    document.getElementById('time').textContent = data.time;
                    document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString('fr-FR');
                })
                .catch(error => {
                    console.error('Error:', error);
                    alert('Erreur de connexion');
                });
        }
        
        // Auto-refresh every 5 seconds
        setInterval(updateData, 5000);
        
        // Initial load
        updateData();
    </script>
</body>
</html>
)rawliteral";

// ==========================================
// CONFIGURATION PAGE
// ==========================================

const char WEBPAGE_CONFIG[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Configuration - Smart LED Clock</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
            background: #f0f0f0;
        }
        .container {
            background: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            text-align: center;
        }
        .nav {
            text-align: center;
            margin-bottom: 20px;
        }
        .nav a {
            color: #4CAF50;
            text-decoration: none;
            margin: 0 10px;
        }
        .form-section {
            margin: 30px 0;
            padding: 20px;
            background: #f9f9f9;
            border-radius: 8px;
        }
        .form-section h2 {
            color: #4CAF50;
            margin-top: 0;
        }
        .form-group {
            margin: 15px 0;
        }
        label {
            display: block;
            margin-bottom: 5px;
            color: #666;
            font-weight: bold;
        }
        input[type="text"],
        input[type="number"],
        input[type="password"],
        input[type="color"],
        select {
            width: 100%;
            padding: 10px;
            border: 1px solid #ddd;
            border-radius: 4px;
            box-sizing: border-box;
        }
        small {
            color: #999;
            font-size: 12px;
        }
        .color-group {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 10px;
        }
        .save-btn {
            display: block;
            width: 100%;
            padding: 15px;
            background: #4CAF50;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 16px;
            margin-top: 20px;
        }
        .save-btn:hover {
            background: #45a049;
        }
        .message {
            padding: 15px;
            border-radius: 5px;
            margin: 20px 0;
            display: none;
        }
        .message.success {
            background: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .message.error {
            background: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>⚙️ Configuration</h1>
        
        <div class="nav">
            <a href="/">← Retour au tableau de bord</a>
        </div>
        
        <div id="message" class="message"></div>
        
        <form id="configForm">
            <!-- NTP Settings -->
            <div class="form-section">
                <h2>🕐 Paramètres Horaires</h2>
                
                <div class="form-group">
                    <label>Fuseau horaire (UTC offset):</label>
                    <input type="number" id="timezoneOffset" min="-12" max="14" required>
                    <small>Exemple: 2 pour UTC+2 (Paris)</small>
                </div>
                
                <div class="form-group">
                    <label>Heure de synchronisation NTP:</label>
                    <input type="number" id="ntpSyncHour" min="0" max="23" required>
                </div>
                
                <div class="form-group">
                    <label>Minute de synchronisation NTP:</label>
                    <input type="number" id="ntpSyncMinute" min="0" max="59" required>
                </div>
            </div>
            
            <!-- LED Colors -->
            <div class="form-section">
                <h2>💡 Couleurs des LED</h2>
                
                <div class="form-group">
                    <label>Couleur des heures:</label>
                    <div class="color-group">
                        <input type="number" id="colorHourR" min="0" max="255" placeholder="R" required>
                        <input type="number" id="colorHourG" min="0" max="255" placeholder="G" required>
                        <input type="number" id="colorHourB" min="0" max="255" placeholder="B" required>
                    </div>
                </div>
                
                <div class="form-group">
                    <label>Couleur des minutes:</label>
                    <div class="color-group">
                        <input type="number" id="colorMinuteR" min="0" max="255" placeholder="R" required>
                        <input type="number" id="colorMinuteG" min="0" max="255" placeholder="G" required>
                        <input type="number" id="colorMinuteB" min="0" max="255" placeholder="B" required>
                    </div>
                </div>
                
                <div class="form-group">
                    <label>Couleur des secondes:</label>
                    <div class="color-group">
                        <input type="number" id="colorSecondR" min="0" max="255" placeholder="R" required>
                        <input type="number" id="colorSecondG" min="0" max="255" placeholder="G" required>
                        <input type="number" id="colorSecondB" min="0" max="255" placeholder="B" required>
                    </div>
                </div>
                
                <div class="form-group">
                    <label>Luminosité (0-255):</label>
                    <input type="number" id="ledBrightness" min="0" max="255" required>
                </div>
            </div>
            
            <!-- LCD Settings -->
            <div class="form-section">
                <h2>📺 Paramètres LCD</h2>
                
                <div class="form-group">
                    <label>Timeout rétroéclairage (secondes):</label>
                    <input type="number" id="lcdTimeout" min="5" max="300" required>
                </div>
            </div>
            
            <button type="submit" class="save-btn">💾 Enregistrer la configuration</button>
        </form>
    </div>
    
    <script>
        // Load current configuration
        function loadConfig() {
            fetch('/api/config')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('timezoneOffset').value = data.timezoneOffset;
                    document.getElementById('ntpSyncHour').value = data.ntpSyncHour;
                    document.getElementById('ntpSyncMinute').value = data.ntpSyncMinute;
                    
                    document.getElementById('colorHourR').value = data.led.hour.r;
                    document.getElementById('colorHourG').value = data.led.hour.g;
                    document.getElementById('colorHourB').value = data.led.hour.b;
                    document.getElementById('colorMinuteR').value = data.led.minute.r;
                    document.getElementById('colorMinuteG').value = data.led.minute.g;
                    document.getElementById('colorMinuteB').value = data.led.minute.b;
                    document.getElementById('colorSecondR').value = data.led.second.r;
                    document.getElementById('colorSecondG').value = data.led.second.g;
                    document.getElementById('colorSecondB').value = data.led.second.b;
                    document.getElementById('ledBrightness').value = data.led.brightness;
                    
                    document.getElementById('lcdTimeout').value = data.lcdTimeout / 1000;
                })
                .catch(error => {
                    showMessage('Erreur de chargement de la configuration', 'error');
                });
        }
        
        // Save configuration
        document.getElementById('configForm').addEventListener('submit', function(e) {
            e.preventDefault();
            
            const config = {
                timezoneOffset: parseInt(document.getElementById('timezoneOffset').value),
                ntpSyncHour: parseInt(document.getElementById('ntpSyncHour').value),
                ntpSyncMinute: parseInt(document.getElementById('ntpSyncMinute').value),
                led: {
                    hour: {
                        r: parseInt(document.getElementById('colorHourR').value),
                        g: parseInt(document.getElementById('colorHourG').value),
                        b: parseInt(document.getElementById('colorHourB').value)
                    },
                    minute: {
                        r: parseInt(document.getElementById('colorMinuteR').value),
                        g: parseInt(document.getElementById('colorMinuteG').value),
                        b: parseInt(document.getElementById('colorMinuteB').value)
                    },
                    second: {
                        r: parseInt(document.getElementById('colorSecondR').value),
                        g: parseInt(document.getElementById('colorSecondG').value),
                        b: parseInt(document.getElementById('colorSecondB').value)
                    },
                    brightness: parseInt(document.getElementById('ledBrightness').value)
                },
                lcdTimeout: parseInt(document.getElementById('lcdTimeout').value) * 1000,
            };
            
            fetch('/api/config', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(config)
            })
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    showMessage('✅ Configuration enregistrée avec succès!', 'success');
                } else {
                    showMessage('❌ Erreur: ' + data.message, 'error');
                }
            })
            .catch(error => {
                showMessage('❌ Erreur de communication', 'error');
            });
        });
        
        function showMessage(text, type) {
            const msg = document.getElementById('message');
            msg.textContent = text;
            msg.className = 'message ' + type;
            msg.style.display = 'block';
            
            setTimeout(() => {
                msg.style.display = 'none';
            }, 5000);
        }
        
        // Load config on page load
        loadConfig();
    </script>
</body>
</html>
)rawliteral";

// ==========================================
// MOON PHASE PAGE
// ==========================================

const char WEBPAGE_MOON[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Phase Lunaire - Smart LED Clock</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
            background: #f0f0f0;
        }
        .container {
            background: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            text-align: center;
        }
        .nav {
            text-align: center;
            margin: 20px 0;
        }
        .nav a {
            color: #4CAF50;
            text-decoration: none;
            font-size: 18px;
        }
        .nav a:hover {
            text-decoration: underline;
        }
        .moon-display {
            text-align: center;
            padding: 40px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            border-radius: 15px;
            color: white;
            margin: 30px 0;
        }
        .moon-phase {
            font-size: 48px;
            margin-bottom: 20px;
        }
        .phase-name {
            font-size: 32px;
            font-weight: bold;
            margin-bottom: 10px;
        }
        .illumination {
            font-size: 24px;
            opacity: 0.9;
        }
        .moon-info {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
            margin: 30px 0;
        }
        .info-card {
            background: #f9f9f9;
            padding: 20px;
            border-radius: 8px;
            border-left: 4px solid #667eea;
        }
        .info-label {
            font-size: 14px;
            color: #666;
            margin-bottom: 5px;
        }
        .info-value {
            font-size: 24px;
            font-weight: bold;
            color: #333;
        }
        .info-unit {
            font-size: 16px;
            color: #999;
        }
        .calibration-section {
            background: #fff3cd;
            padding: 20px;
            border-radius: 8px;
            border: 1px solid #ffc107;
            margin: 20px 0;
        }
        .calibration-section h3 {
            margin-top: 0;
            color: #856404;
        }
        .calib-btn {
            display: block;
            width: 100%;
            padding: 15px;
            background: #ffc107;
            color: #333;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 16px;
            font-weight: bold;
            margin-top: 15px;
        }
        .calib-btn:hover {
            background: #e0a800;
        }
        .calib-btn:disabled {
            background: #ccc;
            cursor: not-allowed;
        }
        .status-good {
            color: #28a745;
            font-weight: bold;
        }
        .status-warning {
            color: #ffc107;
            font-weight: bold;
        }
        .status-error {
            color: #dc3545;
            font-weight: bold;
        }
        .update-time {
            text-align: center;
            color: #999;
            margin-top: 20px;
        }
        .loading {
            text-align: center;
            padding: 40px;
            font-size: 18px;
            color: #666;
        }
        .message {
            padding: 15px;
            border-radius: 5px;
            margin: 20px 0;
            display: none;
        }
        .message.success {
            background: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .message.error {
            background: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🌙 Phase Lunaire</h1>
        
        <div class="nav">
            <a href="/">← Retour au tableau de bord</a>
        </div>
        
        <div id="message" class="message"></div>
        
        <div id="loading" class="loading">
            Chargement des données lunaires...
        </div>
        
        <div id="moonContent" style="display: none;">
            <!-- Moon Visual Display -->
            <div class="moon-display">
                <div class="moon-phase" id="moonEmoji">🌑</div>
                <div class="phase-name" id="phaseName">--</div>
                <div class="illumination">
                    <span id="illumination">--</span>% illuminée
                </div>
            </div>
            
            <!-- Moon Information Cards -->
            <div class="moon-info">
                <div class="info-card">
                    <div class="info-label">Âge Lunaire</div>
                    <div class="info-value" id="lunarAge">--</div>
                    <div class="info-unit">jours</div>
                </div>
                
                <div class="info-card">
                    <div class="info-label">Phase Exacte</div>
                    <div class="info-value" id="exactPhase">--</div>
                    <div class="info-unit">/ 8.0</div>
                </div>
                
                <div class="info-card">
                    <div class="info-label">Position Moteur</div>
                    <div class="info-value" id="motorPosition">--</div>
                    <div class="info-unit">/ 2048 pas</div>
                </div>
                
                <div class="info-card">
                    <div class="info-label">État</div>
                    <div class="info-value" id="calibStatus">--</div>
                </div>
            </div>
            
            <!-- Calibration Section -->
            <div class="calibration-section">
                <h3>⚙️ Calibration</h3>
                <p>
                    <strong>Dernière calibration:</strong> 
                    <span id="lastCalib">--</span>
                </p>
                <p id="calibDetails" style="margin: 10px 0; font-size: 14px; color: #666;">
                    <!-- Calibration details will be inserted here -->
                </p>
                <button class="calib-btn" id="calibBtn" onclick="recalibrateMoon()">
                    🔄 Recalibrer Maintenant
                </button>
                <p style="margin-top: 10px; font-size: 14px; color: #856404;">
                    ⚠️ La recalibration prend environ 40 secondes
                </p>
            </div>
            
            <div class="update-time">
                Dernière mise à jour: <span id="lastUpdate">--</span>
            </div>
        </div>
    </div>
    
    <script>
        // Moon phase emojis
        const MOON_EMOJIS = ['🌑', '🌒', '🌓', '🌔', '🌕', '🌖', '🌗', '🌘'];
        
        // Update moon data
        function updateMoonData() {
            fetch('/api/moon?action=status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('loading').style.display = 'none';
                    document.getElementById('moonContent').style.display = 'block';
                    
                    // Update moon display
                    document.getElementById('moonEmoji').textContent = MOON_EMOJIS[data.phase] || '🌑';
                    document.getElementById('phaseName').textContent = data.phaseName;
                    document.getElementById('illumination').textContent = data.illumination.toFixed(1);
                    
                    // Update info cards
                    document.getElementById('lunarAge').textContent = data.lunarAge.toFixed(2);
                    document.getElementById('exactPhase').textContent = data.exactPhase.toFixed(3);
                    document.getElementById('motorPosition').textContent = data.currentSteps || '--';
                    
                    // Update calibration status
                    const calibStatus = document.getElementById('calibStatus');
                    if (data.calibrated) {
                        calibStatus.textContent = '✓ Calibré';
                        calibStatus.className = 'info-value status-good';
                    } else {
                        calibStatus.textContent = '✗ Non calibré';
                        calibStatus.className = 'info-value status-error';
                    }
                    
                    // Update last calibration
                    if (data.daysSinceCalibration !== undefined) {
                        const days = data.daysSinceCalibration;
                        document.getElementById('lastCalib').textContent = 
                            days < 1 ? "Aujourd'hui" : 
                            days < 2 ? "Hier" :
                            days.toFixed(1) + " jours";
                        
                        // Show calibration details
                        let calibDetails = '';
                        if (days >= 28) {
                            calibDetails = '⚠️ <span class="status-warning">Recalibration recommandée (>28 jours)</span>';
                        } else if (days >= 25) {
                            calibDetails = 'ℹ️ Recalibration bientôt nécessaire';
                        } else {
                            calibDetails = '✓ <span class="status-good">Calibration récente</span>';
                        }
                        document.getElementById('calibDetails').innerHTML = calibDetails;
                    } else {
                        document.getElementById('lastCalib').textContent = 'Jamais';
                        document.getElementById('calibDetails').innerHTML = 
                            '⚠️ <span class="status-error">Calibration initiale requise</span>';
                    }
                    
                    // Update timestamp
                    document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString('fr-FR');
                })
                .catch(error => {
                    console.error('Error fetching moon data:', error);
                    document.getElementById('loading').innerHTML = 
                        '<span style="color: #dc3545;">❌ Erreur de chargement. Module lunaire non disponible?</span>';
                    showMessage('Erreur: Impossible de charger les données lunaires', 'error');
                });
        }
        
        // Recalibrate moon
        function recalibrateMoon() {
            if (!confirm('La recalibration va prendre environ 40 secondes.\nLe module va scanner pour trouver la position de référence.\n\nContinuer?')) {
                return;
            }
            
            const btn = document.getElementById('calibBtn');
            btn.disabled = true;
            btn.textContent = '⏳ Calibration en cours... (40s)';
            
            showMessage('🔄 Calibration en cours... Veuillez patienter 40 secondes', 'success');
            
            fetch('/api/moon?action=recalibrate')
                .then(response => response.json())
                .then(data => {
                    btn.disabled = false;
                    btn.textContent = '🔄 Recalibrer Maintenant';
                    
                    if (data.success) {
                        showMessage(
                            '✅ Calibration réussie!\n' +
                            'Valeur du pic: ' + data.peakValue + ' / 1023\n' +
                            'Durée: ' + (data.duration / 1000).toFixed(1) + 's',
                            'success'
                        );
                        setTimeout(() => updateMoonData(), 2000);
                    } else {
                        showMessage(
                            '❌ Calibration échouée.\n' +
                            'Valeur du pic trop faible: ' + data.peakValue + ' < 300\n' +
                            'Vérifiez le capteur LDR et le disque de calibration.',
                            'error'
                        );
                    }
                })
                .catch(error => {
                    console.error('Error during recalibration:', error);
                    btn.disabled = false;
                    btn.textContent = '🔄 Recalibrer Maintenant';
                    showMessage('❌ Erreur de communication lors de la recalibration', 'error');
                });
        }
        
        // Show message
        function showMessage(text, type) {
            const msg = document.getElementById('message');
            msg.textContent = text;
            msg.className = 'message ' + type;
            msg.style.display = 'block';
            
            setTimeout(() => {
                msg.style.display = 'none';
            }, 8000);
        }
        
        // Initial load
        updateMoonData();
        
        // Auto-refresh every 60 seconds
        setInterval(updateMoonData, 60000);
    </script>
</body>
</html>
)rawliteral";

#endif // WEBPAGE_H