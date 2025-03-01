.model tiny       
.286              
.code             
org 100h  
; 1) ����� ����� ����������� ������� �����������  -> ��������� ��� �� ���� ��� ������ ���� ���� � ��� �� ��
; 2) ��� ����� ������� ���� ���� ��� ����� ����� 
; 3) ��� ������ ��������� 1 �������. � ��������� ���� ����� ������� 
; 4) �� ������ ������� ���� ������ ���������� ����� ��� ����� ��� ��������� ����� ������ ��� ��� ?
; 

; ������ ��� ���� ������������ (��������� �����, ��������� ��������)
; �������� 
; ������� (������ ������������, )
; �������� readme

; ��������������� � ������ ���� 
;


; ������� �������� -> �� ���� �������������� .com 
Start: 
                ; ��������� ��� ������
                mov  si, offset password 
                call elf_hash               ; ������� ������� ������� ��� ������ 
                mov  true_hash, ax      

                ; ������� ����������� ����� ������
                mov dx, offset invitation  
                mov ah, 09h
                int 21h

                ; ��������� ������
                mov  si, offset user_buffer
                call enter_password          

                ; ��������� ��� ���������� ������ 
                mov  si, offset user_buffer
                call elf_hash

                ; ���������� ���� 
                cmp ax, true_hash
                jne fail_case

success_case:
                mov dx, offset success_msg
                mov ah, 09h
                int 21h
                jmp end_case

fail_case:
                mov dx, offset fail_msg
                mov ah, 09h
                int 21h

end_case:
                mov ax, 4c00h
                int 21h
;-----------------------------------------------------

;====================================================================
; ������� ��� ����� ������ 
;====================================================================
enter_password proc
               push bp
               mov  bp, sp
               xor  cx, cx

@@while1:
               mov ah, 01h ; 07h
               int 21h
               cmp al, 0dh
               je @@end_while

               mov [si],al
               inc  si
               inc  cx
               jmp @@while1

@@end_while:
               mov byte ptr [si], 0
               pop bp
               ret

enter_password endp

;====================================================================
; ������� ��� ���������� ���� (ElfHash)
;====================================================================
elf_hash       proc
               push bp
               mov  bp, sp
               xor  ax, ax                          ; �������� ax (����� ����� ��������� ���)
               xor  bx, bx                          ; �������� bx (��������� �������)

@@for_start: 
               mov bl, [si]                        ; ��������� ������ �� ������ � bl
               cmp bl, 0                           ; ���������, ��������� �� ����� ������
               je @@end_for                        ; ���� ��, ��������� ����

               ; �������� elfhash
               shl ax, 4                           ; �������� ��� �� 4 ���� �����
               add ax, bx                          ; ��������� ASCII-��� ������� � ����
               mov bx, ax                          ; �������� ��� � bx
               and bx, 0F000h                      ; �������� ������� 4 ����
               shr bx, 24                          ; �������� �� �� 24 ���� ������
               xor ax, bx                          ; xor � �����
               and ax, 0FFFFh                      ; ��������� ������ ������� 16 ���

               inc si                              ; ��������� � ���������� �������
               jmp @@for_start                     ; ��������� ����

@@end_for:
               pop bp
               ret                                 ; ������������ �� �������
elf_hash       endp

;=====================================================================================================
;                                    CONSTANTS
;=====================================================================================================
invitation  db 'Enter your password: $' 
success_msg db 'Success :)', '$'        
fail_msg    db 'Fail', '$'              
true_hash   dw 0                        

password  db 'hello', 0           ; TODO ���-�� �������� ������       
user_buffer db 5 dup(0)                  

;-----------------------------------------------------------------------

end start