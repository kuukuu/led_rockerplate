# led_rockerplate


1.set serviceUUID empty, like a bellow
  static BLEUUID serviceUUID("");
2.build and run to find your service UUID
3.you will find all BLE device list, check your power source's UUID
4.set your serviceUUID and DEF_BLE_ADDR.
5.set TARGET_CHARA empty and build & run.
5.check characteristic list ofrom your power source.
  XXXXXX63-XXXXXXXX******  is power data.
6.set your TARGET_CHARA
7.set your power zone at setRGB method.