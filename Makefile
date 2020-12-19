#FQBN = adafruit:samd:adafruit_trinket_m0
FQBN = adafruit:avr:protrinket5

install: holiday-lights.ino
	arduino --upload --board $(FQBN) $@
