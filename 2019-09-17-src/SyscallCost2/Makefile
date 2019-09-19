all:	SyscallCost2.exe  

SyscallCost2.exe:   SyscallCost2.c ntquery.obj
    cl /Zi SyscallCost2.c ntquery.obj
   
	
ntquery.obj: ntquery.asm
    ml64 /c ntquery.asm

clean:
	del *.pdb *.obj *.exe *.ilk *.sln *.suo *.sdf