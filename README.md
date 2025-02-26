<h1>P5 Clock GPS</h1>
P5 Clock GPS
P5 LED 3x3 pcs Clock getting time and date from GPS satellite. </br> </br>
To built this clock I used P5 LED RGB. But customer wants only White color in text. 
</br>
The clock screen supposed to looks like this:
</br></br>
<p align="center">
<img src="https://github.com/user-attachments/assets/ff64035f-ae57-4a07-b05b-8dccceedd66b" width="500" />
</p>

Provided drawing for the case. You can find drawing files it in "Solidworks files" directory:

<p align="center">
..pic..
</p>

Connection scheme:

<p align="center">
<img src="https://github.com/user-attachments/assets/7321e080-0147-4685-ae06-c8a4e75eaf1c" width="500">
</p>

Provided PCB for the clock. I used STM32 BluePill development board and Neo-6M GPS-Module. You can find all PCB files in "Altium files" directory. Also you can find PDF-schemes in "Schemes" folder.

<p align="center">
<img src="https://github.com/user-attachments/assets/b49c79e8-c419-483a-976a-4d17150c7053" width="300">
</p>
<p align="center"> PCB front </p>

<p align="center">
<img src="https://github.com/user-attachments/assets/e5926b49-7d95-4f21-a915-c927b1f58c77" width="300">
</p>
<p align="center"> PCB back </p>

Manufactured PCB:
<p align="center">
<img src="https://github.com/user-attachments/assets/cf020749-5679-4ed9-9046-e848b9c40a31" width="450">
</p>

Backside assembly:
<p align="center">
<img src="https://github.com/user-attachments/assets/ed16ec3d-a02d-4f7a-a8eb-474ec6673576" width="700">
</p>

From the front:
<p align="center">
<img src="https://github.com/user-attachments/assets/2038f5e0-b25c-4d08-9906-383e3f05e12e" width="500">
</p>


For programming I used **DMD_STM32 library**. It just works and library owner always provides feedback. </br>
You can download it there: https://github.com/board707/DMD_STM32 . </br>

Two fonts were created: big is to show the time, smaller - to show the date. Both of them are in "Fonts" folder. </br>

Also one thing: better to find Px LED module which is supported by library. Please check up what shift-register ICs are used in your LED module before you order. </br>

The clock uses external antenna. So you have to keep in mind about correct antenna placement - it doesn't get any data from satellite if there is any obstacle between antenna and satellite:

<p align="center">
<img src="https://github.com/user-attachments/assets/9d6fddc6-26b8-480c-962e-d62a42dc47c6" width="500">
</p>

Installed clock photo:
<p align="center">
<img src="https://github.com/user-attachments/assets/63559e83-9454-4a53-973b-467fcb7fae4a" width="500">
</p>






