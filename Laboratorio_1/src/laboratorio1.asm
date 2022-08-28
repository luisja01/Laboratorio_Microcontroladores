;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 4.0.0 #11528 (Linux)
;--------------------------------------------------------
; PIC port for the 14-bit core
;--------------------------------------------------------
;	.file	"laboratorio1.c"
	list	p=12f675
	radix dec
	include "p12f675.inc"
;--------------------------------------------------------
; config word(s)
;--------------------------------------------------------
	__config 0x3fd7
;--------------------------------------------------------
; external declarations
;--------------------------------------------------------
	extern	__moduint
	extern	_TRISIO
	extern	_GPIO
	extern	_GPIObits
	extern	__sdcc_gsinit_startup
;--------------------------------------------------------
; global declarations
;--------------------------------------------------------
	global	_main
	global	_mostrar_num
	global	_rando
	global	_delay

	global PSAVE
	global SSAVE
	global WSAVE
	global STK12
	global STK11
	global STK10
	global STK09
	global STK08
	global STK07
	global STK06
	global STK05
	global STK04
	global STK03
	global STK02
	global STK01
	global STK00

sharebank udata_ovr 0x0020
PSAVE	res 1
SSAVE	res 1
WSAVE	res 1
STK12	res 1
STK11	res 1
STK10	res 1
STK09	res 1
STK08	res 1
STK07	res 1
STK06	res 1
STK05	res 1
STK04	res 1
STK03	res 1
STK02	res 1
STK01	res 1
STK00	res 1

;--------------------------------------------------------
; global definitions
;--------------------------------------------------------
;--------------------------------------------------------
; absolute symbol definitions
;--------------------------------------------------------
;--------------------------------------------------------
; compiler-defined variables
;--------------------------------------------------------
UDL_laboratorio1_0	udata
r0x1014	res	1
r0x1015	res	1
r0x1017	res	1
r0x1016	res	1
r0x1019	res	1
r0x1018	res	1
r0x101A	res	1
r0x1011	res	1
r0x1010	res	1
r0x1013	res	1
r0x1012	res	1
r0x100B	res	1
r0x100A	res	1
r0x100D	res	1
r0x100C	res	1
r0x100E	res	1
r0x100F	res	1
r0x1003	res	1
r0x1002	res	1
r0x1004	res	1
r0x1005	res	1
r0x1006	res	1
r0x1007	res	1
r0x1008	res	1
r0x1009	res	1
;--------------------------------------------------------
; initialized data
;--------------------------------------------------------

IDD_laboratorio1_0	idata
_rando_rand_65536_16
	db	0xe1, 0xac	; 44257

;--------------------------------------------------------
; initialized absolute data
;--------------------------------------------------------
;--------------------------------------------------------
; overlayable items in internal ram 
;--------------------------------------------------------
;	udata_ovr
;--------------------------------------------------------
; reset vector 
;--------------------------------------------------------
STARTUP	code 0x0000
	nop
	pagesel __sdcc_gsinit_startup
	goto	__sdcc_gsinit_startup
;--------------------------------------------------------
; code
;--------------------------------------------------------
code_laboratorio1	code
;***
;  pBlock Stats: dbName = M
;***
;has an exit
;functions called:
;   _rando
;   _rando
;   _mostrar_num
;   _delay
;   _mostrar_num
;   _delay
;   _mostrar_num
;   _delay
;   _mostrar_num
;   _delay
;   _mostrar_num
;   _delay
;   _mostrar_num
;   _delay
;   _rando
;   _rando
;   _mostrar_num
;   _delay
;   _mostrar_num
;   _delay
;   _mostrar_num
;   _delay
;   _mostrar_num
;   _delay
;   _mostrar_num
;   _delay
;   _mostrar_num
;   _delay
;10 compiler assigned registers:
;   r0x1014
;   r0x1015
;   STK02
;   STK01
;   STK00
;   r0x1016
;   r0x1017
;   r0x1018
;   r0x1019
;   r0x101A
;; Starting pCode block
S_laboratorio1__main	code
_main:
; 2 exit points
;	.line	15; "laboratorio1.c"	TRISIO = 0b00001000; //Poner el pin 3 como entrada y los demás como salidas
	MOVLW	0x08
	BANKSEL	_TRISIO
	MOVWF	_TRISIO
;	.line	16; "laboratorio1.c"	GPIO = 0x00; //Poner pines en bajo
	BANKSEL	_GPIO
	CLRF	_GPIO
;	.line	20; "laboratorio1.c"	unsigned int cuenta = 0 ;
	CLRF	r0x1014
	CLRF	r0x1015
_00116_DS_:
;	.line	26; "laboratorio1.c"	num1 = rando(0,9);
	MOVLW	0x09
	MOVWF	STK02
	MOVLW	0x00
	MOVWF	STK01
	MOVLW	0x00
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_rando
	CALL	_rando
	PAGESEL	$
	MOVWF	r0x1016
	MOVF	STK00,W
	MOVWF	r0x1017
;	.line	27; "laboratorio1.c"	num2 = rando(0,9); 
	MOVLW	0x09
	MOVWF	STK02
	MOVLW	0x00
	MOVWF	STK01
	MOVLW	0x00
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_rando
	CALL	_rando
	PAGESEL	$
	MOVWF	r0x1018
	MOVF	STK00,W
	MOVWF	r0x1019
;	.line	29; "laboratorio1.c"	if (GP3 ==1)
	CLRF	r0x101A
	BANKSEL	_GPIObits
	BTFSC	_GPIObits,3
	INCF	r0x101A,F
	MOVF	r0x101A,W
	XORLW	0x01
	BTFSS	STATUS,2
	GOTO	_00106_DS_
;	.line	31; "laboratorio1.c"	cuenta =+cuenta+1;
	INCF	r0x1014,F
	BTFSC	STATUS,2
	INCF	r0x1015,F
_00106_DS_:
;	.line	34; "laboratorio1.c"	if (cuenta ==17)
	MOVF	r0x1014,W
	XORLW	0x11
	BTFSS	STATUS,2
	GOTO	_00112_DS_
	MOVF	r0x1015,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00112_DS_
;	.line	36; "laboratorio1.c"	cuenta = 0;  
	CLRF	r0x1014
	CLRF	r0x1015
_00107_DS_:
;	.line	37; "laboratorio1.c"	while (GP3==1)
	CLRF	r0x101A
	BANKSEL	_GPIObits
	BTFSC	_GPIObits,3
	INCF	r0x101A,F
	MOVF	r0x101A,W
	XORLW	0x01
	BTFSS	STATUS,2
	GOTO	_00109_DS_
;	.line	38; "laboratorio1.c"	mostrar_num(9,0);
	MOVLW	0x00
	MOVWF	STK02
	MOVLW	0x00
	MOVWF	STK01
	MOVLW	0x09
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_mostrar_num
	CALL	_mostrar_num
	PAGESEL	$
	GOTO	_00107_DS_
_00109_DS_:
;	.line	39; "laboratorio1.c"	delay(5);
	MOVLW	0x05
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_delay
	CALL	_delay
	PAGESEL	$
;	.line	40; "laboratorio1.c"	mostrar_num(9,1);
	MOVLW	0x01
	MOVWF	STK02
	MOVLW	0x00
	MOVWF	STK01
	MOVLW	0x09
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_mostrar_num
	CALL	_mostrar_num
	PAGESEL	$
;	.line	41; "laboratorio1.c"	delay(5);
	MOVLW	0x05
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_delay
	CALL	_delay
	PAGESEL	$
_00112_DS_:
;	.line	45; "laboratorio1.c"	while (GP3 == 1)
	CLRF	r0x101A
	BANKSEL	_GPIObits
	BTFSC	_GPIObits,3
	INCF	r0x101A,F
	MOVF	r0x101A,W
	XORLW	0x01
	BTFSS	STATUS,2
	GOTO	_00114_DS_
;	.line	47; "laboratorio1.c"	mostrar_num(num1,0);
	MOVLW	0x00
	MOVWF	STK02
	MOVLW	0x00
	MOVWF	STK01
	MOVF	r0x1017,W
	MOVWF	STK00
	MOVF	r0x1016,W
	PAGESEL	_mostrar_num
	CALL	_mostrar_num
	PAGESEL	$
;	.line	48; "laboratorio1.c"	delay(5);
	MOVLW	0x05
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_delay
	CALL	_delay
	PAGESEL	$
;	.line	49; "laboratorio1.c"	mostrar_num(num2,1);
	MOVLW	0x01
	MOVWF	STK02
	MOVLW	0x00
	MOVWF	STK01
	MOVF	r0x1019,W
	MOVWF	STK00
	MOVF	r0x1018,W
	PAGESEL	_mostrar_num
	CALL	_mostrar_num
	PAGESEL	$
;	.line	50; "laboratorio1.c"	delay(5); 
	MOVLW	0x05
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_delay
	CALL	_delay
	PAGESEL	$
	GOTO	_00112_DS_
_00114_DS_:
;	.line	53; "laboratorio1.c"	mostrar_num(num1,0);
	MOVLW	0x00
	MOVWF	STK02
	MOVLW	0x00
	MOVWF	STK01
	MOVF	r0x1017,W
	MOVWF	STK00
	MOVF	r0x1016,W
	PAGESEL	_mostrar_num
	CALL	_mostrar_num
	PAGESEL	$
;	.line	54; "laboratorio1.c"	delay(5);
	MOVLW	0x05
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_delay
	CALL	_delay
	PAGESEL	$
;	.line	55; "laboratorio1.c"	mostrar_num(num2,1);
	MOVLW	0x01
	MOVWF	STK02
	MOVLW	0x00
	MOVWF	STK01
	MOVF	r0x1019,W
	MOVWF	STK00
	MOVF	r0x1018,W
	PAGESEL	_mostrar_num
	CALL	_mostrar_num
	PAGESEL	$
;	.line	56; "laboratorio1.c"	delay(5);
	MOVLW	0x05
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_delay
	CALL	_delay
	PAGESEL	$
	GOTO	_00116_DS_
;	.line	60; "laboratorio1.c"	}
	RETURN	
; exit point of _main

;***
;  pBlock Stats: dbName = C
;***
;has an exit
;9 compiler assigned registers:
;   r0x1002
;   STK00
;   r0x1003
;   r0x1004
;   r0x1005
;   r0x1006
;   r0x1007
;   r0x1008
;   r0x1009
;; Starting pCode block
S_laboratorio1__delay	code
_delay:
; 2 exit points
;	.line	137; "laboratorio1.c"	void delay(unsigned int tiempo)
	MOVWF	r0x1002
	MOVF	STK00,W
	MOVWF	r0x1003
;	.line	142; "laboratorio1.c"	for(i=0;i<tiempo;i++)
	CLRF	r0x1004
	CLRF	r0x1005
_00215_DS_:
	MOVF	r0x1002,W
	SUBWF	r0x1005,W
	BTFSS	STATUS,2
	GOTO	_00236_DS_
	MOVF	r0x1003,W
	SUBWF	r0x1004,W
_00236_DS_:
	BTFSC	STATUS,0
	GOTO	_00217_DS_
;;genSkipc:3307: created from rifx:0x7ffd7fb16d80
;	.line	143; "laboratorio1.c"	for(j=0;j<1275;j++);
	MOVLW	0xfb
	MOVWF	r0x1006
	MOVLW	0x04
	MOVWF	r0x1007
_00213_DS_:
	MOVLW	0xff
	ADDWF	r0x1006,W
	MOVWF	r0x1008
	MOVLW	0xff
	MOVWF	r0x1009
	MOVF	r0x1007,W
	BTFSC	STATUS,0
	INCFSZ	r0x1007,W
	ADDWF	r0x1009,F
	MOVF	r0x1008,W
	MOVWF	r0x1006
	MOVF	r0x1009,W
	MOVWF	r0x1007
	MOVF	r0x1009,W
	IORWF	r0x1008,W
	BTFSS	STATUS,2
	GOTO	_00213_DS_
;	.line	142; "laboratorio1.c"	for(i=0;i<tiempo;i++)
	INCF	r0x1004,F
	BTFSC	STATUS,2
	INCF	r0x1005,F
	GOTO	_00215_DS_
_00217_DS_:
;	.line	144; "laboratorio1.c"	}
	RETURN	
; exit point of _delay

;***
;  pBlock Stats: dbName = C
;***
;has an exit
;functions called:
;   __moduint
;   __moduint
;9 compiler assigned registers:
;   r0x100A
;   STK00
;   r0x100B
;   STK01
;   r0x100C
;   STK02
;   r0x100D
;   r0x100E
;   r0x100F
;; Starting pCode block
S_laboratorio1__rando	code
_rando:
; 2 exit points
;	.line	116; "laboratorio1.c"	unsigned int rando(unsigned int start_range,unsigned int end_range)
	MOVWF	r0x100A
	MOVF	STK00,W
	MOVWF	r0x100B
	MOVF	STK01,W
	MOVWF	r0x100C
	MOVF	STK02,W
;	.line	121; "laboratorio1.c"	if(start_range == end_range) {
	MOVWF	r0x100D
	XORWF	r0x100B,W
	BTFSS	STATUS,2
	GOTO	_00186_DS_
	MOVF	r0x100C,W
	XORWF	r0x100A,W
	BTFSS	STATUS,2
	GOTO	_00186_DS_
;	.line	122; "laboratorio1.c"	return start_range;
	MOVF	r0x100B,W
	MOVWF	STK00
	MOVF	r0x100A,W
	GOTO	_00190_DS_
_00186_DS_:
;	.line	126; "laboratorio1.c"	rand += 0x3AD;
	MOVLW	0xad
	ADDWF	_rando_rand_65536_16,F
	MOVLW	0x03
	BTFSC	STATUS,0
	MOVLW	0x04
	ADDWF	(_rando_rand_65536_16 + 1),F
;	.line	127; "laboratorio1.c"	rand %= end_range;
	MOVF	r0x100D,W
	MOVWF	STK02
	MOVF	r0x100C,W
	MOVWF	STK01
	MOVF	_rando_rand_65536_16,W
	MOVWF	STK00
	MOVF	(_rando_rand_65536_16 + 1),W
	PAGESEL	__moduint
	CALL	__moduint
	PAGESEL	$
	MOVWF	(_rando_rand_65536_16 + 1)
	MOVF	STK00,W
	MOVWF	_rando_rand_65536_16
_00187_DS_:
;	.line	130; "laboratorio1.c"	while(rand < start_range){
	MOVF	r0x100A,W
	SUBWF	(_rando_rand_65536_16 + 1),W
	BTFSS	STATUS,2
	GOTO	_00204_DS_
	MOVF	r0x100B,W
	SUBWF	_rando_rand_65536_16,W
_00204_DS_:
	BTFSC	STATUS,0
	GOTO	_00189_DS_
;;genSkipc:3307: created from rifx:0x7ffd7fb16d80
;	.line	131; "laboratorio1.c"	rand = rand + end_range - start_range;
	MOVF	r0x100D,W
	ADDWF	_rando_rand_65536_16,W
	MOVWF	r0x100E
	MOVF	(_rando_rand_65536_16 + 1),W
	MOVWF	r0x100F
	MOVF	r0x100C,W
	BTFSC	STATUS,0
	INCFSZ	r0x100C,W
	ADDWF	r0x100F,F
	MOVF	r0x100B,W
	SUBWF	r0x100E,W
	MOVWF	_rando_rand_65536_16
	MOVF	r0x100A,W
	BTFSS	STATUS,0
	INCF	r0x100A,W
	SUBWF	r0x100F,W
	MOVWF	(_rando_rand_65536_16 + 1)
	GOTO	_00187_DS_
_00189_DS_:
;	.line	134; "laboratorio1.c"	return rand;
	MOVF	_rando_rand_65536_16,W
	MOVWF	STK00
	MOVF	(_rando_rand_65536_16 + 1),W
_00190_DS_:
;	.line	135; "laboratorio1.c"	}
	RETURN	
; exit point of _rando

;***
;  pBlock Stats: dbName = C
;***
;has an exit
;7 compiler assigned registers:
;   r0x1010
;   STK00
;   r0x1011
;   STK01
;   r0x1012
;   STK02
;   r0x1013
;; Starting pCode block
S_laboratorio1__mostrar_num	code
_mostrar_num:
; 2 exit points
;	.line	62; "laboratorio1.c"	void mostrar_num (int numero, int display)
	MOVWF	r0x1010
	MOVF	STK00,W
	MOVWF	r0x1011
	MOVF	STK01,W
	MOVWF	r0x1012
	MOVF	STK02,W
;	.line	66; "laboratorio1.c"	if (display == 0)
	MOVWF	r0x1013
	IORWF	r0x1012,W
	BTFSS	STATUS,2
	GOTO	_00178_DS_
;	.line	68; "laboratorio1.c"	if (numero == 0) GPIO = 0b00000000;
	MOVF	r0x1010,W
	IORWF	r0x1011,W
	BTFSS	STATUS,2
	GOTO	_00146_DS_
	BANKSEL	_GPIO
	CLRF	_GPIO
	GOTO	_00180_DS_
_00146_DS_:
;	.line	70; "laboratorio1.c"	else if (numero == 1) GPIO = 0b00000001; 
	MOVF	r0x1011,W
	XORLW	0x01
	BTFSS	STATUS,2
	GOTO	_00143_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00143_DS_
	MOVLW	0x01
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00143_DS_:
;	.line	72; "laboratorio1.c"	else if (numero == 2) GPIO = 0b00000010; 
	MOVF	r0x1011,W
	XORLW	0x02
	BTFSS	STATUS,2
	GOTO	_00140_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00140_DS_
	MOVLW	0x02
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00140_DS_:
;	.line	74; "laboratorio1.c"	else if (numero == 3) GPIO = 0b00000011;
	MOVF	r0x1011,W
	XORLW	0x03
	BTFSS	STATUS,2
	GOTO	_00137_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00137_DS_
	MOVLW	0x03
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00137_DS_:
;	.line	76; "laboratorio1.c"	else if (numero == 4) GPIO = 0b00000100;
	MOVF	r0x1011,W
	XORLW	0x04
	BTFSS	STATUS,2
	GOTO	_00134_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00134_DS_
	MOVLW	0x04
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00134_DS_:
;	.line	78; "laboratorio1.c"	else if (numero == 5) GPIO = 0b00000101;
	MOVF	r0x1011,W
	XORLW	0x05
	BTFSS	STATUS,2
	GOTO	_00131_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00131_DS_
	MOVLW	0x05
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00131_DS_:
;	.line	80; "laboratorio1.c"	else if (numero == 6) GPIO = 0b00000110;
	MOVF	r0x1011,W
	XORLW	0x06
	BTFSS	STATUS,2
	GOTO	_00128_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00128_DS_
	MOVLW	0x06
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00128_DS_:
;	.line	82; "laboratorio1.c"	else if (numero == 7) GPIO = 0b00000111;
	MOVF	r0x1011,W
	XORLW	0x07
	BTFSS	STATUS,2
	GOTO	_00125_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00125_DS_
	MOVLW	0x07
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00125_DS_:
;	.line	84; "laboratorio1.c"	else if (numero == 8) GPIO = 0b00010000;
	MOVF	r0x1011,W
	XORLW	0x08
	BTFSS	STATUS,2
	GOTO	_00122_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00122_DS_
	MOVLW	0x10
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00122_DS_:
;	.line	86; "laboratorio1.c"	else GPIO = 0b00010001;
	MOVLW	0x11
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00178_DS_:
;	.line	89; "laboratorio1.c"	else if (display == 1)
	MOVF	r0x1013,W
	XORLW	0x01
	BTFSS	STATUS,2
	GOTO	_00180_DS_
	MOVF	r0x1012,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00180_DS_
;	.line	91; "laboratorio1.c"	if (numero == 0) GPIO = 0b00100000;
	MOVF	r0x1010,W
	IORWF	r0x1011,W
	BTFSS	STATUS,2
	GOTO	_00173_DS_
	MOVLW	0x20
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00173_DS_:
;	.line	93; "laboratorio1.c"	else if (numero == 1) GPIO = 0b00100001; 
	MOVF	r0x1011,W
	XORLW	0x01
	BTFSS	STATUS,2
	GOTO	_00170_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00170_DS_
	MOVLW	0x21
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00170_DS_:
;	.line	95; "laboratorio1.c"	else if (numero == 2) GPIO = 0b00100010; 
	MOVF	r0x1011,W
	XORLW	0x02
	BTFSS	STATUS,2
	GOTO	_00167_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00167_DS_
	MOVLW	0x22
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00167_DS_:
;	.line	97; "laboratorio1.c"	else if (numero == 3) GPIO = 0b00100011;
	MOVF	r0x1011,W
	XORLW	0x03
	BTFSS	STATUS,2
	GOTO	_00164_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00164_DS_
	MOVLW	0x23
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00164_DS_:
;	.line	99; "laboratorio1.c"	else if (numero == 4) GPIO = 0b00100100;
	MOVF	r0x1011,W
	XORLW	0x04
	BTFSS	STATUS,2
	GOTO	_00161_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00161_DS_
	MOVLW	0x24
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00161_DS_:
;	.line	101; "laboratorio1.c"	else if (numero == 5) GPIO = 0b00100101;
	MOVF	r0x1011,W
	XORLW	0x05
	BTFSS	STATUS,2
	GOTO	_00158_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00158_DS_
	MOVLW	0x25
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00158_DS_:
;	.line	103; "laboratorio1.c"	else if (numero == 6) GPIO = 0b00100110;
	MOVF	r0x1011,W
	XORLW	0x06
	BTFSS	STATUS,2
	GOTO	_00155_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00155_DS_
	MOVLW	0x26
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00155_DS_:
;	.line	105; "laboratorio1.c"	else if (numero == 7) GPIO = 0b00100111;
	MOVF	r0x1011,W
	XORLW	0x07
	BTFSS	STATUS,2
	GOTO	_00152_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00152_DS_
	MOVLW	0x27
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00152_DS_:
;	.line	107; "laboratorio1.c"	else if (numero == 8) GPIO = 0b00110000;
	MOVF	r0x1011,W
	XORLW	0x08
	BTFSS	STATUS,2
	GOTO	_00149_DS_
	MOVF	r0x1010,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00149_DS_
	MOVLW	0x30
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00180_DS_
_00149_DS_:
;	.line	109; "laboratorio1.c"	else GPIO = 0b00110001;
	MOVLW	0x31
	BANKSEL	_GPIO
	MOVWF	_GPIO
_00180_DS_:
;	.line	113; "laboratorio1.c"	}
	RETURN	
; exit point of _mostrar_num


;	code size estimation:
;	  454+   55 =   509 instructions ( 1128 byte)

	end
