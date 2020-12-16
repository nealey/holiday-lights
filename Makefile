FQBN = adafruit:samd:adafruit_trinket_m0

install: holiday-lights.ino
	arduino --upload --board $(FQBN) $@
