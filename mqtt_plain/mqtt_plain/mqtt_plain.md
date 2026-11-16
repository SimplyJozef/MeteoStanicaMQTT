# mqtt_plain – výstup zo sériového monitora a z brokera

Doska ESP32 (COM6), 16. 11. 2026, 21:33:49. Broker Mosquitto 2.1.2 na PC v domácej sieti, port 1883 bez šifrovania a bez hesla. Téma `meteo/meteo-01/telemetry`, správa každých 5 s.

## ESP32 (sériový monitor)

```
PUB {"dev":"meteo-01","seq":59,"ts_ms":359636,"temp_c":21.61,"rh_pct":53.6,"press_hpa":986.74,"lux":2.5,"aqi":3,"tvoc_ppb":248,"eco2_ppm":749,"ens_status":2,"rain_raw":4095,"rain":false}
PUB {"dev":"meteo-01","seq":60,"ts_ms":364636,"temp_c":21.62,"rh_pct":53.6,"press_hpa":986.73,"lux":2.5,"aqi":3,"tvoc_ppb":226,"eco2_ppm":725,"ens_status":2,"rain_raw":4095,"rain":false}
PUB {"dev":"meteo-01","seq":61,"ts_ms":369636,"temp_c":21.60,"rh_pct":53.5,"press_hpa":986.75,"lux":2.5,"aqi":2,"tvoc_ppb":216,"eco2_ppm":713,"ens_status":2,"rain_raw":4095,"rain":false}
PUB {"dev":"meteo-01","seq":62,"ts_ms":374636,"temp_c":21.61,"rh_pct":53.6,"press_hpa":986.75,"lux":2.5,"aqi":3,"tvoc_ppb":251,"eco2_ppm":753,"ens_status":2,"rain_raw":4095,"rain":false}
PUB {"dev":"meteo-01","seq":63,"ts_ms":379636,"temp_c":21.58,"rh_pct":53.6,"press_hpa":986.73,"lux":2.5,"aqi":3,"tvoc_ppb":238,"eco2_ppm":738,"ens_status":2,"rain_raw":4095,"rain":false}
PUB {"dev":"meteo-01","seq":64,"ts_ms":384636,"temp_c":21.58,"rh_pct":53.5,"press_hpa":986.73,"lux":2.5,"aqi":3,"tvoc_ppb":241,"eco2_ppm":742,"ens_status":2,"rain_raw":4095,"rain":false}
```

## PC (`mosquitto_sub -h localhost -t "meteo/#" -v`)

```
meteo/meteo-01/telemetry {"dev":"meteo-01","seq":59,"ts_ms":359636,"temp_c":21.61,"rh_pct":53.6,"press_hpa":986.74,"lux":2.5,"aqi":3,"tvoc_ppb":248,"eco2_ppm":749,"ens_status":2,"rain_raw":4095,"rain":false}
meteo/meteo-01/telemetry {"dev":"meteo-01","seq":60,"ts_ms":364636,"temp_c":21.62,"rh_pct":53.6,"press_hpa":986.73,"lux":2.5,"aqi":3,"tvoc_ppb":226,"eco2_ppm":725,"ens_status":2,"rain_raw":4095,"rain":false}
meteo/meteo-01/telemetry {"dev":"meteo-01","seq":61,"ts_ms":369636,"temp_c":21.60,"rh_pct":53.5,"press_hpa":986.75,"lux":2.5,"aqi":2,"tvoc_ppb":216,"eco2_ppm":713,"ens_status":2,"rain_raw":4095,"rain":false}
meteo/meteo-01/telemetry {"dev":"meteo-01","seq":62,"ts_ms":374636,"temp_c":21.61,"rh_pct":53.6,"press_hpa":986.75,"lux":2.5,"aqi":3,"tvoc_ppb":251,"eco2_ppm":753,"ens_status":2,"rain_raw":4095,"rain":false}
meteo/meteo-01/telemetry {"dev":"meteo-01","seq":63,"ts_ms":379636,"temp_c":21.58,"rh_pct":53.6,"press_hpa":986.73,"lux":2.5,"aqi":3,"tvoc_ppb":238,"eco2_ppm":738,"ens_status":2,"rain_raw":4095,"rain":false}
meteo/meteo-01/telemetry {"dev":"meteo-01","seq":64,"ts_ms":384636,"temp_c":21.58,"rh_pct":53.5,"press_hpa":986.73,"lux":2.5,"aqi":3,"tvoc_ppb":241,"eco2_ppm":742,"ens_status":2,"rain_raw":4095,"rain":false}
meteo/meteo-01/telemetry {"dev":"meteo-01","seq":65,"ts_ms":389636,"temp_c":21.57,"rh_pct":53.6,"press_hpa":986.73,"lux":2.5,"aqi":3,"tvoc_ppb":241,"eco2_ppm":742,"ens_status":2,"rain_raw":4095,"rain":false}
```
