# real-time-kernel
Realtime kernel for MS-DOS, written in C and 80x86 Assembly

Original text, written in 1995 in Dutch

Real Time Kernel
Version 0.1
Jeroen van der Waal March 1996 

Doel
----

Dit project behelst de ontwikkeling van een real time operating systeem. De
ontwikkeling heeft meerdere doelen: ten eerste de ins en outs bestuderen van
een rt-os, ten tweede als deelproject voor een toekomstig te maken ontwikkel
tool voor embeded software. Dit toekomstig project zal waarschijnlijk ESDK
gaan heten (Embeded Software Development Kit).


Requirements
------------

Minimaal worden de volgende eisen aan de kernel gesteld:

xx. kunnen draaien op een embedded systeem.
    Alle elementaire dingen welke nodig zijn om stand-alone te kunnen
    functioneren dienen aanwezig te zijn. Bijvoorbeeld een memory manager.
xx. kunnen simuleren alsof het in een embedded systeem draait. Hierbij kan
    gedacht worden aan een host in de vorm van een unix werkstation
xx. De applicatie code voor bovenstaande twee punten moet ongewijzigd blijven
    voor zowel het embedded als het gesimuleerde systeem.
xx. de kernel moet processen kunnen creeeren en beeindigen.
xx. wordt in de vorm van een bibliotheek aan een applicatie gebonden. Koppeling
    van deze bibliotheek moet in principe met elke ansi c compiler kunnen.
xx. beschikt over een manier om abrupt het hele systeem te beeindigen.
xx. scheduling van processen moeten zodanig van aard zijn dat deze
    tegemoet komen aan real-time eisen.
xx. Moet over een mechanisme beschikken om taken in slaap te brengen
    (suspending) en weer te wekken. De periode kan voor onbepaalde tijd zijn
    (blocked) of voor bepaalde tijd (timed).
xx. instaat zijn interrupts af te handelen. Twee manieren zijn mogelijk:
    een interrupt wekt een process voor verwerking, of een speciale
    interrupthandler wordt rechtstreeks aangeroepen.
xx. voorzien zijn van een timerfunctie. De  timer wordt middels een standaard 
    driver aangestuurd. 
xx. Interproces communicatie
    op drie manieren kunnen taken gegevens met elkaar uitwisselen
    - semaforen
    Semaforen kunnen taken blokkeren voor bepaalde en onbepaalde tijd. Er dient 
    rekening gehouden te worden dat meerdere processen wachten op 1 semafoor. Ook
    kunnen signalen van verschillende taken afkomstig zijn.
        Drie verschillende typen semaforen onderscheiden zich:
        1 binair        
        2 counter
        3 resource
        ad 1    Een binaire semafoor kan slechts twee toestanden bezitten. De semafoor
                is 0 of 1. Dit heeft tot gevolg dat het sturen van een signaal naar
                een semafoor die al een waarde had van een geen verandering van de
                semafoor tot gevolg heeft (deze blijft 1). Signalen kunnen bij dit
                type dus verloren gaan. Zie [1].

        ad 2    Een counter semafoor is niet begrensd tot 1. Iedere keer wanneer
                een semafoor wordt gesignaleerd zal de semafoor waarde met een
                verhoogd worden. Maximaal aantal signalen dat op deze manier bewaard
                kunnen worden bedraagd 64k-1.

        ad 3    Precieze omschrijving nog niet duidelijk. Deze semafoor lijkt het meest
                op een counter semafoor. Met dit verschil dat prioriteiten van wachtende
                taken beinvloed worden.


    - mail boxes
    Mailboxen voorzien in gebufferde berichtgeving tussen taken. Bij
    initialisatie kan de grootte van deze buffer eenmalig ingesteld worden.
    Ten eerste wordt ingesteld hoe groot een message is, ten tweede wordt 
    aangegeven hoeveel van deze boodschappen in de buffer moeten passen.	
    Taken kunnen berichten in de buffer plaatsen en berichten uit de buffer
    nemen. De buffer werkt als een FIFO. Op twee manieren kan een taak
    in slaap geraken geraken. De eerste manier is indien een taak een message
    probeert te plaatsen in een volle mailbox. De taak wordt gestopt tot
    op het moment dat er weer ruimte is in de mailbox. Evenzo wordt een
    taak opgehouden indien het een boodschap probeert te pakken uit een
    lege mailbox. De taak wordt hervat als er daadwerkelijk een boodschap
    aanwezig is.
    Ter voorkoming dat teveel taken in- en uitgeschakeld worden gedurende
    mailbox communicatie, kan een threshold worden ingesteld waarop taken
    worden hervat. Bijvoorbeeld een taak welke boodschappen plaats in een
    mailbox wordt raakt op een gegeven moment 'suspended' omdat de box vol
    is. Een andere taak haalt een boodschap uit de mailbox, en wekt de andere
    slapende taak. Deze kan nu weer een boodschap plaatsen aen zal weer
    in slaaptoestand geraken. Veel wisselingen in de toestand van de taak
    welke boodschappen plaatst. Dit kan voorkomen worden door een threshold
    in te stellen. De zendende taak wordt pas weer aktief indien er in de
    mailbox N vrije posities zijn. Is zo'n constructie voor de ontvanger
    ook mogelijk ?
    Let op! het kan mogelijk zijn dat deze constructie niet gewenst is.
    Indien direkt verhandeling van een message gewenst is.


    - message passing
    Uitwisseling kan plaatsvinden waarbij processen geblokt, tijdelijk
    geblokt of in het geheel niet geblokt worden.
    Implementatie van semaforen dient extra aandacht besteed worden aan de
    de snelheid van uitvoer. Omdat semaforen de basis vormen voor alle andere 
    vormen van IPC, is de snelheid van deze andere vormen mede bepaald hierdoor.
    Snelle sema's dragen dus bij tot een algemene snelle IPC.

xx. streven naar een zo groot mogelijke onafhankelijkheid betreffende processor
    en hardware architectuur. Onafhankelijkheid voor hardware wordt zoveel
    mogelijk verkregen door te werken met drivers. Het is de bedoeling de kernel
    voor diverse kleine processoren geschikt te maken. Processoren zoals de z80,
    68000, 80x86 moeten ondersteund gaan worden.

xx. Informatie kunnen geven over toestanden van processen. Informatie over
    volgende zaken:
    - status van de taak (bijvoorbeeld ready, waiting, ...)
    - geconsumeerde tijd door specifiek taak.
    - maximale en minimale stack gebruik (voor debug doeleinden).
    - prioriteit van de taak

xx. Gestandaardiseerd, uniform, interface voor drivers hebben.
        Drivers hebben tot doel hardware te koppelen aan de kernel. Drivers
        moeten een zodanige opzet hebben dat naast koppeling met echte hard
        ware ook mogelijkheden zijn voor het simuleren van hardware. Hierbij
        kan gedacht worden uit het putten van informatie uit een file.
        Drivers kunnen I/O pollende drivers (een taak) of interrupt
        gestuurd zijn. Voor simulatie moet het mogelijk zijn interrupt
        bronnen te immiteren.
        Een aantal drivers moeten standaard aanwezig zijn: voor serieele
        communicatie ala RS232 een parallelle poort aansturing, misschien
        een harddisk driver ...

xx. Beschikken over memory manager, geoptimaliseerd voor snelheid.
        De memory manager moet werken volgens het algoritme best-fit.
        Daarnaast beschikt de manager de mogelijkheid te werken met RAM
        gebieden welke niet aaneengesloten zijn. Ook moet het mogelijk zijn
        RAM gebieden classificeren naar bijvoorbeeld Battery-Backed-up,
        dynamisch, statisch-slow and statische fast, allocatie voor
        geheugen naar aanvraag naar grootte.
        Deze laatste optie voorkomt fragmentatie van het geheugen en doet dat
        als volgd: het geheugen wordt in bijvoorbeeld drie delen gesplitst.
        Het eerste deel wordt gebruikt voor memory allocaties ter grootte van
        maximaal 16 bytes. Het tweede gebied voor allocaties ter grootte van
        bijvoorbeeld 128 bytes. Tenslotte wordt het laatste deel gebruikt
        voor allocaties groter dan 128 bytes.
        

        Voor simulatie van de memory manager moet deze gebruik kunnen
        maken van het host-geheugen. Door van de host een stuk geheugen
        te nemen, kan deze het uitdelen alsof het het geheugen van het
        embedded systeem betreft.


xx. Beschikken over een fouten afhandelingssysteem, debug mogelijkheden
    via bijvoorbeeld een RS232 poort.
    De kernel moet daarom in twee smaken verschijnen: een speciale debug
    versie en een versie optimaal voor snelheid en code afmeting. De versie
    met debug info wordt gebruikt tijden apllicatie ontwikkeling. Fouten
    van de programmeur worden zoveel mogelijk gedetecteerd en gemeld. Indien
    een applicatie het finale stadium heeft bereikt wordt de no-debug kernel
    gebruikt en zal de applicatie kleiner en sneller worden.
    Naast dat de kernel berichten stuurt over dit debug systeem, is het voor
    de applicatieprogrammeur mogelijk dit systeem aan te wenden voor eigen
    gebruik. M.a.w. zowel de kernel, systeem-modules alsmede applicatie
    fouten worden door een centraal foutafhandelingssysteem kenbaar gemaakt
    aan de buitenwereld.



A S S E M B L A G E   V O O R S C H R I F T
-------------------------------------------

Algemeen

Bij implementatie van de kernel wordt zoveel mogelijk gebruik gemaakt van
abstracte data types. Alle operaties welke betrekking hebben op lijsten
zijn ondergebracht in lists.c/lists.h. Een tweede abstract data type zijn
queueus. Zie [2]. Gebruikt voor implementatie van mail boxes.

Ten behoeve van de platvorm en processor afhankelijkheid wordt zoveel
mogelijk code in ansi C geschreven.


De basis van de kernel is de 'kernelbase' (Zie kernel.h). Deze basis
houdt de administratie van alle processen bij. Administratie gebeurt
middels linked lists. De kernelbase beschikt over een aantal lijsten:
slsReadyList, slsTimedList, slsWaitList.
- slsReadyList: hierin zijn de taeken opgenomen die op dit moment uitvoerbaar
  zijn.
- slsTimedList: taken welke tijdelijk suspended zijn maar zeker na bepaalde
  tijd weer de status ready zullen krijgen.
- slsWaitList: taken die enkel door een extern event gewekt kunnen worden.

Task table
In de task table worden alle voor de taak belangrijke zaken onthouden. Iedere
taak beschikt over een eigen stack. Deze stack wordt zowel gebruikt door de
taak alsmede door de kernel. Indien een taak tijdens een scheduling operatie
gestopt wordt, dan wordt de processor status op deze stack opgeslagen. Omdat
'C' hiervoor geen mogelijkheden biedt, is dit stuk in assembly geschreven
(processor afhankelijk). Voor administratiee doeleinden beschikt de tabel over
twee node's genaamd primary en secondary link. Deze node's zijn een basis
element van het abstracte data type lists. De primary link wordt altijd
gebruikt om de taak in een drie hierboven vermelde lijsten op te nemen. De
tweede link wordt alleen gebruikt indien er sprake is interprocescommunicatie.
Meer over de secondary link in het gedeelte over interprocescommunicatie.
Het veld u16State in de tabel geeft aan welke toestand de taak op dit moment
bezit. Zie in de header kernel.h de verschillende status mogelijkheden (bijvoorbeed
STATE_TIMEDPUT). Zie ook verder in deze tekst.
Het timer veld wordt gebruikt voor acties waarbij een taak gewekt kan worden
door het aflopen van een timer. Funkties als rtkDelay en IPC functies maken
veelvuldig van dit veld gebruik (ZIE IPC).
De MSGPASS wordt gebruikt voor IPC op basis van message passing.

de task-tabel: het status veld
Het statusveld geeft aan in welke toestand een taak bezit. Vaak voorkomend
zijn de termen BLOCKED en TIMED in de status benoeming. BLOCKED wil zeggen
dat een taak zolang blijft wachten totdat de reden van blokkering vervallen
is. TIMED wil zeggen dat de taak gewekt wordt of door het vervallen van de
blokkeringsreden of door het aflopen van een timer. De taak zal in ieder
geval gewekt worden.
De volgende
toestanden zijn gedefinieerd:

  state             Omschrijving
  =====             ============
        
1  STATE_READY       klaar om uitgevoerd te worden
2  STATE_RUNNING     de taak heeft op dit moment de beschikking over de processor
3  STATE_INITIALIZE  nieuwe taak die net is aangemeld en is klaar om uitgevoerd
                     te worden.
4  STATE_KILLED      taak is afgelopen en zal nooit meer aktief worden.
5  STATE_DELAYING    slapende taak, welke na bepaalde tijd weer gewekt zal
                     worden.
6  STATE_BLOCKEDWAIT door wait() (down) operatie op semafoor geblokkeerde taak.
7  STATE_TIMEDWAIT   idem, echter nu is de blokkade van tijdelijke aard. Na het 
                     verstrijken van een bepaalde tijd (time-out) wordt ondanks 
                     dat semafoor geblokkeerd is de taak toch gewekt. 
8  STATE_BLOCKEDPUT  het plaatsen van een message in een volle mailbox heeft
                     deze taak geblokkeerd.
9  STATE_BLOCKEDGET  taak geblokkeerd door het proberen een boodschap te
                     pakken uit lege mailbox.
10 STATE_TIMEDPUT    idem 
11 STATE_TIMEDGET     


Slapen en wekken van taken
--------------------------

Taken kunnen slapen voor bepaalde en onbepaalde tijd. Voor taken die een
bepaalde tijd slapen wordt een timer functie gebruikt. Deze timerfunctie
maakt integraal deel uit van de kernel.
Een taak welke gereed is om uit te voeren bevindt zich in de 'Ready List'.
Twee lijsten zijn er om taken te registreren welke of voor onbepaalde tijd
, of voor bepaalde tijd zijn ge-suspend.
Het in slaap brengen van een taak is niet meer dan het verplaatsen van een
taak van de ene lijst naar de andere.

Het verplaatsen van een taak kan gedaan worden door de scheduler. Indien een
taak niet verder uitgevoerd kan worden, wordt de status van die taak veranderd
en wordt de scheduler aangeroepen. Deze onderneemt acties aan de hand van de
nieuwe status.
Bijvoorbeeld een lopende taak wil zichzelf voor bepaalde tijd opschorten.
Hiertoe roept deze de functie Delay() aan. De functie delay doet in
principe twee dingen: veranderd de status van een taak van READY naar DELAYED
en roept daarbij de scheduler aan. In de scheduler is een mechanisme aanwezig
dat de taak uit de ready lijst verhuist naar de waiting list.



Interprocescommuncatie (semaforen als basis voor alle IPC)
----------------------

Zoals het er nu naar uitziet blijft enkel de semafoor bestaan en zal message 
passing en de mailbox veranderen. De operaties voor de mailbox zijn:
- rtkCreateSema(S, V)   Initialize semafore with value V
- rtkDeleteSema(S)	delete resources allocated for semafore
- rtkSemaValue(S)	get current semafore value
- rtkSignal(S)		signal semafoor S (increment semafore)
- rtkWait(S)		wait for signal   (decrement semafore)
- rtkWaitCond(S) 	only if possible decrement semafore, don't wait
- rtkWaitTimed(S, T)	same as rtkWait only with time out of T time units
Deze set van operaties kan als sub-class worden beschouwd voor IPC. Vraag is
nu hoe met deze sub-class IPC varianten 'mailbox' en 'message passing' 
gerealiseerd kunnen worden.

Structuur elementen in de semafoor
UINT16 SemaValue
LIST slsWaitTasksList

Pseudo code rtkWait(S)

VOID rtkWait(S)
begin
    disable interrupts
    if ( SemaValue > 0 )
    begin
        decrement SemaValue
    end
    else begin
        breng taak die deze functie aanriep in slaap
    end
    enable interrupts 
end



1. realisatie van mailboxen
Gebruik van abstract data 'FIFO' queue (zie [2]). Operaties als insert() en 
get(). Twee semaforen beschermen de inhoud van de queue. De eerste semafoor
wordt gebruikt voor 'er is ruimte voor tenminste 1 bericht in de FIFO. Ten 
tweede is er een semafoor die aangeeft dat er tenminste 1 bericht aanwezig
is in de mailbox.

In het data type queue zijn de volgende functies voor handen:
- CreateQueue(MaxItems, Datalen)	Allocatie resources + initialisatie
- RemoveQueue(Q) ** veranderen in DeleteQueue() ** Geef resources weer vrij
- AppendQueue(Q, data)	plaats nieuwe data achteraan in de rij	
- ServeQueue(Q, item)	pak voorste item uit de rij
- QueueFull(Q)		geeft aan of de queue vol is
- QueueEmpty(Q)		geeft aan of de queue leeg is
- QueueCount(Q)		geeft het aantal items aanwezig in de queue


mail box functies:
- rtkCreateMailbox(slots, data size)      	Initialize mailbox M
- rtkPut(MAILBOX, message)			Put message in MAILBOX
- rtkPutTimed(MAILBOX, data, time out)		Same as put but with time out
- rtkGet(MAILBOX, data)				Get message out of mailbox
- rtkGetTimed(MAILBOX, data, time out)		same but with time out


Pseudo code
Voor een mailbox worden twee abstracte data typen gebruikt: semafoor en FIFO
queue. 
In de structuur komem twee semaforen voor: 
	pssmGetMail	Geeft aan of er mail in de queue aanwezig is.
	pssmPutMail	Geeft aan of er tenminste 1 message in de queue 
			geplaatst kan worden.
	psquMessages	Message queue 


Mailbox CreateMailbox(slots, data_size)
begin
	Initialize FIFO psquMessages with params slots and data_size
	Initialize semafore pssmGetMail with a value of 0
	Initailize semafore pssmPutMail with param slots
end


DeleteMailbox(MAILBOX)
begin
	test if mailbox is empty (call QueueEmpty)
	if so do nothing !! diplay an error or something like that

	else
	free allocated resources	
end


rtkPut (MAILBOX, data)
begin
	'test semafore if it is possible to append data to the que'
	e.g. rtkWait(pssmPutMail)
	When this instruction fails the calling task is put to sleep until
	a signal operation is achieved.

	'insert data in the queue'
	achieved by AppendQueue(psquMessages, data)

	'update semafore for retrieving messages'
	rtkSignal(pssmPutMail)
end


rtkGet	(MAILBOX, data)
begin
	pend semafore for mail in the mailbox
	rtkWait(pssmGetMail)
	
	get data from message queue
	ServeQueue(psquMessages, data)

	update 'places available in queue semafore' 
	Signal(pssmPutMail)
end

/* GA NA OF ER GEEN DEADLOCK SITUATIES KUNNEN VOORKOMEN !!!!! */


2. realisatie van message passing

Message passing is een communicatie middel waarbij twee taken
gesynchronisserd worden. Dit kan bereikt worden door drie semaforen die
de volgende functie vervullen:
- Ontvanger is gereed te communiceren
- Zender is gereed te communiceren
- overdracht data tussen ontvanger en zender gereed.
De volgende twee situaties kunnen zich voordoen: de ontvangende taak is klaar 
om data te ontvangen en de zender taak niet en andersom.

het arsenaal functies voor de implementatie van message passing bestaat uit:
- rtkSend(task_id, data)  			zendt een bericht naar taak met id task_id
- boolean rtkSendCond(task_id, data)		 
- boolean rtkSendTimed(task_id, data, time_out)
- rtkReceive(data, data_len)
- boolean rtkReceivedCond(data, data_len)
- boolean rtkRevceivedTimed(data, data_len)
NOTE: initilisatie is niet nodig: deze wordt gedaan door rtkCreateTask.


Pseudo code

gebruikte data types: drie semaforen en een data pointer. De eerste semafoor 
geeft aan of de zender klaar is voor gegevens overdracht. De tweede semafoor 
doet dit voor de ontvangende taak. De derde semafoor geeft aan dat de overdracht
van data voltooid is. 
Deze structuur is opgenomen in de task_table.

rtkSend(task_id, data)
begin
	zoek task_table behorende bij task_id op.
	geef pointer door naar 'data'
	laat ontvanger weten dat zender klaar is voor dataoverdracht
	'rtkSignal(pssmSendReady)'

	wacht totdat ontvanger klaar is voor overdracht.
	'rtkWait(pssmReceiveReady)'
	
	wacht totdat transmissie voltooid is
	'rtkWait(pssmDone)'	
end


rtkRevceive(data, data_len)
begin
	laat ontvangende taak weten dat ontvanger klaar is voor het
	ontvangen van data
	rtkSignal(pssmReceiverReady)

	wacht totdat de zendende taak gereed is voor data overdracht
	rtkWait(pssmReceiverReady)

	copy data from pointer set by sending task

	laat zendende taak verder gaan
	rtkSignal(pssmDone)
end


Initialisatie van de kernel
---------------------------

De kernel wordt d.m.v. een funktie rtkInit geinitialiseerd. Interruptvectoren
worden gewijzigd, twee taken worden geinitieerd: MainTask en IdleTask.
Idletask is een taak die niets uitvoerd. Hij heeft altijd de status ready en
wordt gescheduled indien er geen andere taken uitvoerbaar zijn. De idle task
kan indien de processor daar mogelijkheden toe heeft de processor voor enige
tijd stil leggen (power done mode). Er wordt dan minder energie verbruikt
door processor en dit is voordelig bij battery gevoede apparaten.
De MainTask is de taak welke ontstaat door omzetting van de main() routine
in een aparte taak.






Status van de software
----------------------
Op dit moment bestaat is er een werkend geheel, die aan een aantal van de
requirements voldoet. De software werkt enkel nog voor een PC met als
ontwikkelomgeving borland 3.1 IDE. Een aantal testprogramma's zijn gemaakt
die onder andere mailboxes, semaforen, timer en .... test.
Ook ben ik nog niet tevreden met de structuur van de software: met name de
module ipc.c staat me tegen.

De module ipc.c kan voorkomen worden door niet alle interprocescommunicatie 
(IPC) te laten verlopen via aparte routines. Bij de huidige implementatie 
is telkens weer code geschreven om processen te re-schedule-n. Beter zou zijn 
indien semaforen de basis zouden zijn voor alle vormen van IPC.
Toelichting:
1. Mailbox
Een mailbox kan door middel van twee semaforen gereguleerd worden. De eerste
semafoor staat voor 'er is plaats voor tenminste 1 bericht in de mailbox'. De
andere semafoor staat voor 'er is tenminste 1 bericht in de mailbox'.
2. Message Passing
???


Voorlopige plannen...
- aanpassen IPC naar nieuwe ideeen 
- Memory manager
- Standaard driver
- interrupt handling
- De kernel verhuizen naar LINUX platform




Referenties
-----------

[1]     Operating Systemen                      Andrew S. Tanenbaum
        Uitgever
        ISBN XXX

[2]     Programming with data structures        AUTEUR
        Uitgever
        ISBN XXX


