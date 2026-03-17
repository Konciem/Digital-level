#Elektroniczna Poziomica Cyfrowa (Digital Spirit Level)

Projekt elektronicznej poziomicy opartej na mikrokontrolerze Raspberry Pi Pico RP2040. Urządzenie mierzy kąt nachylenia w dwóch osiach (X i Y) w czasie rzeczywistym, wyświetla wyniki na ekranie OLED.

Całość zasilana jest ogniwem Li-Ion z możliwością ładowania i monitorowania stanu baterii, a elektronika zamknięta jest w obudowie z druku 3D.

## Główne Funkcje

* **Pomiar w czasie rzeczywistym:** Wyświetlanie kąta nachylenia w pionie i poziomie.
* **Sygnalizacja poziomu (0°):**
    * Sygnał dźwiękowy (Buzzer).
    * Sygnał wibracyjny (Haptic feedback).
* **Kalibracja:** Funkcja zerowania kąta (tarowania) za pomocą przycisku.
* **Offset:** Możliwość ustawienia własnego punktu odniesienia.
* **System zasilania:** Wbudowane ładowanie USB oraz **odczyt procentowego stanu naładowania baterii** na ekranie.
* **Interfejs:** Obsługa za pomocą 3 przycisków fizycznych.

## Wykaz Elementów

### Mikrokontroler i Czujniki
* **Raspberry Pi Pico** (RP2040) lub Pico W.
* **MPU6050** – 3-osiowy akcelerometr i 3-osiowy żyroskop (komunikacja I2C, 3.3V).

### Wyświetlacz i Sygnalizacja
* **OLED 1.3" (SH1106)** lub 0.96" (SSD1306) – interfejs I2C.
* **Buzzer aktywny** (3.3V – 5V, np. KY-012).
* **Silnik wibracyjny 3V** (typu "coin" lub z telefonu) sterowany przez **tranzystor NPN (np. 2N2222)** + rezystor bazy 1kΩ.

### Zasilanie i Monitorowanie Baterii
* **Ogniwo Li-Ion 3.7V** (np. 18650 lub płaskie ogniwo Li-Po).
* **Moduł ładowania TP4056** (z zabezpieczeniem USB-C/MicroUSB).
* **Przetwornica Step-Up** (podnosząca napięcie do 5V dla VSYS Pico lub stabilne 3.3V).
* **Włącznik suwakowy** (odcinający zasilanie z baterii).
* **Dzielnik napięcia (do pomiaru baterii):**
    * 2x Rezystor (np. 10kΩ i 10kΩ lub 100kΩ i 100kΩ) podłączone do pinu ADC w celu bezpiecznego pomiaru napięcia > 3.3V.

### Sterowanie i Inne
* 4x **Przycisk Tact Switch** (6x6 mm) – (Menu / Kalibracja / Reset/ Stan bateri).
* Rezystory
* Obudowa z druku 3D.

## Schemat połączeń

| Komponent | Pin w Pico (Sugerowany) | Uwagi |
| :--- | :--- | :--- |
| **MPU6050 SDA** | GP4 (I2C0 SDA) | |
| **MPU6050 SCL** | GP5 (I2C0 SCL) | |
| **OLED SDA** | GP4 (I2C0 SDA) | Wspólna magistrala z MPU |
| **OLED SCL** | GP5 (I2C0 SCL) | Wspólna magistrala z MPU |
| **Buzzer** | GP15 | Sterowanie PWM lub High/Low |
| **Wibracja (Baza)** | GP14 | Przez tranzystor! |
| **Przycisk 1** | GP16 | Kalibracja |
| **Przycisk 2** | GP17 | Offset |
| **Przycisk 3** | GP18 | Funkcja dodatkowa |
| **Pomiar Baterii** | GP26 (ADC0) | Przez dzielnik napięcia (V_bat -> R1 -> ADC -> R2 -> GND) |


## Obudowa 3D

Projekt obudowy został zaprojektowany tak, aby pomieścić wszystkie komponenty i zapewnić stabilną podstawę do pomiarów.