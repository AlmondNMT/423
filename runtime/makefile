CC=unicon -s -c
LINK=unicon 

all: comp link exec

comp: runtime.icn random.icn math.icn utils.icn
	$(CC) runtime.icn random.icn math.icn utils.icn

link: runtime.u random.u math.u utils.u 
	unicon runtime.u random.u math.u utils.u test.icn -o testmain

clean:
	rm -f *.u

exec: testmain
	./testmain
