                .data
A:              .word   1, 3, 1, 6, 4
                .word   2, 4, 3, 9, 5
mult:   .word   0

        .code
        daddi   $1, $0, A      ; *A[0]
        daddi   $5, $0, 1      ; $5 = 1 ;; i    
        daddi   $6, $0, 10     ; $6 = N ;; N = 10
        
        lw      $9, 0($1)      ; $9 = A[0] ;; mult
        lw      $10, 8($1)     ; $10 = A[1] 
        lw      $11, 16($1)    ; $11 = A[2] 
        lw      $12, 24($1)    ; $12 = A[3] 

loop:   dmul    $13, $9, $10    ;  $13 = A[i]*A[i+1]
        daddi   $1, $1, 24      ;  $1 = &A[i+3]
        lw      $10, 8($1)      ;  $10 = A[i+3+1]
        dadd    $9, $9, $13     ;  mult = mult + A[i]*A[i+1]

        daddi   $5, $5, 1      ; i += 1

        dmul    $14, $11, $9    ; $14 = A[i+2]*mult
        lw      $11, 16($1)     ; $11 = A[i+3+2]
        dadd    $9, $9, $14     ; mult = mult + A[i]*A[i+1] + A[i+2]*mult

        daddi   $5, $5, 1       ; i += 1

        dmul    $15, $12, $9    ; $15 = A[i+3]*mult
        lw      $12, 24($1)     ; $12 = A[i+3+3]
        dadd    $9, $9, $15     ; mult = mult + A[i]*A[i+1] + A[i+2]*mult + A[i+3]*mult

        daddi   $5, $5, 1       ; i += 1

        bne     $6, $5, loop    ; Exit loop if i == N
        
        sw      $9, mult($0)    ; Store result
        halt

;; Expected result: mult = f6180 (hex), 1008000 (dec)
