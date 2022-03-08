# Wemos D1 mit einem Geigerzähler und einem Telegram Bot

Dieses Projekt habe ich aus spass gestartet, da ich die ESP Boards kennen lernen wollte. 
Ich bin kein Programmierer und habe keine Vorkentnisse in C++. 

### Geiegerzähler

Der verwendete Geigerzähler wurde auf [Aliexpress](https://de.aliexpress.com/item/4000229395987.html?gatewayAdapt=glo2deu&spm=a2g0s.9042311.0.0.27424c4dRx2EwI) bestellt.
Alle Informationen zum Geigerkit können hier eingesehen weden: [GeigerCounter-V1.1](https://github.com/2969773606/GeigerCounter-V1.1)
Damit alles auf dem Wemos D1 funktioniert musste ich ein bisschen vom usprungs Code abweichen.

### Wemos D1

Da der Geigerzähler für das Arduino Uno R3 gedacht ist, muss dieser beim Wemos D1 an einen anderen Pin angeschlossen werden.
Der Zähler verwendet Interrupts und muss somit an den Interrupt Pin angebracht werden.

### Telegram Bot

Der Telegrambot funktioniert mit der [UniversalTelegramBot](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot) Library. 
Diese wurde ebenfalls von mir modifiziert, um stille Benachrichtigungen zu ermöglichen.

# License

The MIT License

Copyright (c) 2022 c4lcu10n

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
