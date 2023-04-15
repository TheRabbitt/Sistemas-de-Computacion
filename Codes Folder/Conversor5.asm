;Este código realiza la operación de multiplicación de dos números de punto flotante de 64 bits 
;y almacena el resultado en el registro xmm0. Los valores de entrada se encuentran en los 
;registros xmm0 y xmm1, y la salida se almacena en xmm0. La función "calc_conversion" realiza 
;esta operación y devuelve el control al programa principal mediante la instrucción "ret".

section .text
    global calc_conversion

calc_conversion:
    mulsd xmm0, xmm1  ; Multipica a por b, y el resultado lo guarda en xmm0
    ret               ; Retorna

