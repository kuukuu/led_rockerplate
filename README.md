# led_rockerplate


- 1.set serviceUUID empty, like a bellow
  - static BLEUUID serviceUUID("");
- 2.build and run to find your service UUID
- 3.you will find all BLE device list, check your power source's UUID
- 4.set your serviceUUID and DEF_BLE_ADDR.
- 5.set TARGET_CHARA empty and build & run.
- 6.check characteristic list ofrom your power source.
  - XXXXXX63-XXXXXXXX******  is power data.
- 7.set your TARGET_CHARA
- 8.set your power zone at setRGB method.

# tested device
- M5stack gray
  M5stack official ver.1.0.6
- WS2811 LED tape
- 12V 3A ac adapter
- wahoo kickr core
- 
# demo
- demo movie on twitter.
  - https://twitter.com/kaa/status/1485603884265766918
- image on strava.
  - https://www.strava.com/activities/6574036423

