# Junior Level Labs Project Description

Persistence Of Vision is the phrase used to describe how our brain tends to continue to 
'see' an image beyond the time the photons from the image are actually striking the retina 
of the eye. We were building phénakisticopes since 1832 and zoetropes since 1834 to 
create the illusion of continuous motion.  The development of projection film in 1888 
was an extension that allowed longer sequences of perceived motion and larger numbers 
of simultaneous viewers.

You will be developing a POV clock with LEDs as the creator of images. It will be an 
analog dial with both hour hand and minute hand.  It will be powerful enough to create 
alternate images, if desired.

## Create Executable Binaries

Execute:
```
make all
```

## Usage

### Static RPi Diagnostic

* By default, execute:
```
sudo ./diagpov
```
and you will be prompted for a specific test number.
* With argument, execute:
```
sudo ./diagpov [test_num]
```
where `[test_num]` is the test number.

### Main Static RPi POV binary

* By default, execute:
```
sudo ./pov
```
and you will be prompted for the rotor RPi IP address.
* With argument, execute:
```
sudo ./pov [rotor_ip]
```
where `[rotor_ip]` is the rotor RPi IP address.

## Create Documentation
Execute:
```
make doc
```
The created documentation main page is [here](../../doc/html/index.html)