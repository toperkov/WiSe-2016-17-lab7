# Bežične senzorske mreže - Lab 7

### FESB, smjer 110/111/112/114/120, akademska godina 2016/2017

U sklopu današnje vježbe student će realizirati jednostavnu bežičnu senzorsku mrežu *stablaste topologije* (``tree topology``).

Mreža će se sastojati od jednog **centralnog uređaja**, **posredničkih bežičnih senzorskih uređaja** te **krajnjih senzorskih uređaja**. Svi uređaji će imati jedinstvenu adresu. Posrednički i krajnji senzorski uređaji su opremljeni sa senzorima temperature/vlage **DHT11/22** i osvjetljenosti **BH1750** te radio primopredajnikom **nRF24L01**. Senzorski podaci se transmitiraju s periodom od 8-10 sekundi prema centralnom uređaju koji te podatke ispisuje na serijski port.

## Mrežna topologija stablaste strukture realizirana pomoću nRF24L01 (+)

U ovoj vježbi cilj je realizirati mrežnu topologiju stablaste stukture. Da bismo to napravili, koristit ćemo biblioteku *RF24Network* koju je potrebno prethodno instalirati u PlatformIO projekt. Više o biblioteci možete naći na  http://tmrh20.github.io/RF24Network/. Svatko tko je upoznat s IP mrežom trebao bi biti u stanju lako razumjeti RF24Network topologiju. Glavni čvor se može vidjeti kao pristupnik, s do 5 izravno povezanih čvorova. Svaki od tih čvorova stvara podmrežu ispod nje, s do 5 dodatnih child čvorova. Način numeriranja također se sličan IP adresiranju.

U RF24Network, glavni čvor ima adresu 00  
Child-ovi glavnog čvora su 01,02,03,04,05  
Child-ovi čvora 01 su 011,021,031,041,051  

### Routiranje (usmjeravanje) podataka

Ako su mrežne adrese dodijeljene u skladu s fizičkim izgledom mreže, čvorovi će automatski usmjeravati promet prema potrebi. Korisnici jednostavno sastavljaju zaglavlje s odgovarajućom odredišnom adresom, a mreža će ga prosljeđivati na ispravan čvor.

Ako se podaci usmjeravaju između parent i child čvorova, tada mreža koristi ugrađene funkcije slanja ACK poruke retransmisija kako bi se spriječio gubitak podataka. Usmjeravanje se upravlja kombinacijom automatiziranih ACK paketa koji su predefinirani *nRF* mikrokontrolerom te softverskih ACK-ova. To omogućava veom brzo slanje podataka svim čvorovima na način da samo krajnji čvor za usmjeravanje potvrđuje isporuku i šalje potvrdu.

Primjer: čvor 00 šalje čvoru 01. Čvorovi će koristiti ugrađene funkcije automatskog pokušaja i automatskog ACK-a.  
Primjer: čvor 00 šalje čvoru 011. Čvor 00 će poslati čvoru 01 kao i prethodno. Čvor 01 će proslijediti poruku na 011. Ako je isporuka bila uspješna, čvor 01 će također proslijediti poruku o uspješnom prijenosu natrag prema čvoru 00.

## Posrednički i krajnji senzorski uređaj

Vaš zadatak je spojiti posredničke i krajnje senzorske uređaje prema slici:

<img src="https://cloud.githubusercontent.com/assets/8695815/24838259/eed6ec80-1d44-11e7-8137-7fabad4a0e53.png" width="400px" />

