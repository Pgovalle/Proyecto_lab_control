decomentar la linea Serial.println(WiFi.localIP());

subir el contenido de la carpeta sockkk al photon utilizando DFU

el photon deber�a "respirar verde", si no es as� y este "flashea verde",las credenciales de la red wifi son incorrectas, vuelva a probar con "particle setup" en cmd.

luego revisar el puerto serial del photon, deber�a enviar una ip.
(puede agregar un delay(1000);)

La ip que env�a el photon p�guela en el archivo python (UDP_IP)

Corra el Python, env�e una "i" y se deber�a prender un led, env�e
otra letra y se deber�a apagar el led.
