decomentar la linea Serial.println(WiFi.localIP());

subir el contenido de la carpeta sockkk al photon utilizando DFU

el photon debería "respirar verde", si no es así y este "flashea verde",las credenciales de la red wifi son incorrectas, vuelva a probar con "particle setup" en cmd.

luego revisar el puerto serial del photon, debería enviar una ip.
(puede agregar un delay(1000);)

La ip que envía el photon péguela en el archivo python (UDP_IP)

Corra el Python, envíe una "i" y se debería prender un led, envíe
otra letra y se debería apagar el led.
