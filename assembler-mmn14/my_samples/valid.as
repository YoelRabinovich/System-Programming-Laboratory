; Basic example

IAmData: .data -99
IAmString: .string "beep boop"
IamCode: mov r5, r1

.entry IAmData

jmp &IAmData
jmp &IAmString

prn IAmData
prn IAmString

