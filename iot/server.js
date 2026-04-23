const express = require('express');
const cors = require('cors');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

// Root route to serve index.html
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

let sensorData = {
  temperature: null,
  humidity: null,
  distance: null,
  timestamp: null,
  history: []
};

const MAX_HISTORY = 50;

app.post('/api/sensor', (req, res) => {
  const { temperature, humidity, distance } = req.body;
  
  sensorData.temperature = temperature;
  sensorData.humidity = humidity;
  sensorData.distance = distance;
  sensorData.timestamp = new Date().toISOString();
  
  sensorData.history.push({
    temperature,
    humidity,
    distance,
    timestamp: sensorData.timestamp
  });

  console.log(`[Sensor Data Received] Temp: ${temperature}, Hum: ${humidity}, Dist: ${distance}`);
  
  if (sensorData.history.length > MAX_HISTORY) {
    sensorData.history.shift();
  }
  
  res.json({ success: true, data: sensorData });
});

app.post('/api/reset', (req, res) => {
  sensorData = {
    temperature: null,
    humidity: null,
    distance: null,
    timestamp: null,
    history: []
  };
  console.log('[Sensor Data Reset]');
  res.json({ success: true, message: 'Data reset successfully' });
});

app.get('/api/sensor', (req, res) => {
  res.json(sensorData);
});

app.get('/api/history', (req, res) => {
  res.json(sensorData.history);
});

app.listen(PORT, () => {
  console.log(`IoT Dashboard running at http://localhost:${PORT}`);
});