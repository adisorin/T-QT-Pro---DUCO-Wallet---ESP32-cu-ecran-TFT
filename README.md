# T-QT-Pro---DUCO-Wallet---ESP32-cu-ecran-TFT
<img width="1080" height="2340" alt="image" src="https://github.com/user-attachments/assets/aebdc3be-ea29-4385-b781-b059334bf871" />

## Prezentare publică – DUCO Wallet pe ESP32 cu ecran TFT

Astăzi vă prezint un proiect embedded realizat pe **ESP32**, care transformă o placă de dezvoltare într-un mini-wallet vizual pentru Duino-Coin, cu afișare pe ecran TFT color.

Proiectul este publicat sub licența MIT License, ceea ce înseamnă că poate fi folosit, modificat și distribuit liber.

---

# 🎯 Ce face acest proiect?

Dispozitivul:

* Se conectează automat la una dintre rețelele WiFi salvate
* Se sincronizează cu un server NTP pentru ora exactă
* Interoghează API-ul oficial Duino-Coin
* Afișează pe ecran:

  * Soldul DUCO
  * Ora curentă
  * Status WiFi
  * Indicator grafic pentru puterea semnalului

Totul rulează autonom, fără intervenție după pornire.

---

# 🖥️ Hardware utilizat

### 🔹 ESP32

Microcontrolerul principal care:

* gestionează WiFi
* face cereri HTTP
* procesează JSON
* controlează display-ul

### 🔹 Display TFT 128x128 cu controller GC9107

Controlat prin biblioteca Arduino_GFX_Library, care permite:

* desenare text
* forme grafice
* control iluminare backlight
* randare eficientă

---

# 🌐 Conectivitate WiFi inteligentă

Se folosește `WiFiMulti`, ceea ce înseamnă:

* Se pot defini mai multe rețele
* ESP32 se conectează automat la cea disponibilă
* Reconectare automată dacă se pierde semnalul

Pe ecran apare:

* 🟢 „WiFi Connected”
* 🔴 „WiFi Connecting...”

În colțul din dreapta sus apare un indicator grafic de semnal, bazat pe valoarea RSSI.

---

# 🕒 Sincronizare automată a orei

După conectarea la internet:

* Se configurează NTP prin serverul `pool.ntp.org`
* Se setează fusul orar România (EET/EEST)
* Ceasul este actualizat la fiecare secundă
* Ora este afișată mare, jos pe ecran

Acest lucru transformă dispozitivul și într-un mini ceas de birou.

---

# 💰 Integrare API Duino-Coin

La fiecare 60 de secunde:

1. Se face cerere HTTP către:

   ```
   https://server.duinocoin.com/balances/username
   ```
2. Se primește răspuns JSON
3. Se extrage:

   ```
   result.balance
   ```
4. Se afișează soldul cu 2 zecimale

Dacă apare o problemă:

* Se afișează „API error” în roșu

---

# 🎨 Interfața grafică

Interfața este minimalistă și eficientă:

* Fundal negru
* Titlu „DUCO Wallet”
* Sold afișat central
* Ceas jos
* Indicator WiFi sus dreapta

Se folosesc culori RGB565 pentru performanță optimă pe microcontroler.

---

# ⚙️ Arhitectura software

Codul este organizat clar pe funcții:

* `drawStaticUI()` → desen interfață inițială
* `checkWiFi()` → verificare conexiune
* `afiseazaCeas()` → afișare oră
* `updateDucoBalance()` → apel API
* `afiseazaNivelWiFi()` → indicator semnal

Bucla principală (`loop`) nu folosește `delay()`, ci `millis()` pentru multitasking non-blocking.

Asta înseamnă:

* Sistem fluid
* Fără blocaje
* Actualizări independente

---

# 🚀 De ce este util acest proiect?

* Monitorizare rapidă a soldului DUCO
* Proiect educațional IoT
* Exemplu complet de:

  * WiFi + HTTP
  * Parsare JSON
  * NTP
  * UI embedded
  * Programare non-blocantă

Poate fi baza pentru:

* Dashboard crypto
* Mini terminal IoT
* Status monitor pentru diverse API-uri

---

# 📌 Concluzie

Acest proiect demonstrează cum un simplu ESP32 poate deveni:

* un wallet vizual
* un ceas sincronizat online
* un dispozitiv IoT complet funcțional

Este un exemplu excelent de integrare între:

* hardware
* rețea
* API web
* interfață grafică

Un proiect mic, dar complet.
