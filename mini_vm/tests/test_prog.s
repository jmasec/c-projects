.global _start  

.section .text
_start:
    MOV R0, #5         
    MOV R1, #10         
    ADD R2, R0, R1      
    SUB R3, R2, #3      

    LDR R4, =data       
    LDR R5, [R4]        

loop:
    SUBS R3, R3, #1    
    BNE loop          

    BL function_call 

    B _start       

function_call:
    ADD R6, R6, #1   
    BX LR           

.section .data
data:
    .word 42