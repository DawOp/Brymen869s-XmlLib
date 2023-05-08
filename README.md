# Brymen869s-XmlLib
A library that allows reading measurements of digital multimeter Brymen869s and parse them into XML

<img src="Docs/multimeter.png" width="200">



## Dependencies

* [Ahid library](http://ahidlib.com/pages/programming_cpp.php?lang=en) - USB HID driver device for Windows.

* [TinyXml2](https://github.com/leethomason/tinyxml2) - c++ xml parser. 

## Usage

**Lib(x86)** contains all needed files to use Brymen869s-XmlLib for 64-bit Windows 64.

**Docs** contains information details about communication with digital multimeter. 


Brymen_registerCallback() - parse measurements from multimeter screen into xml and invokes any function passed as callback on xml measurements.

Brymen_start() - starts a connection with the multimeter.

Brymen_end() turns off a connection.

