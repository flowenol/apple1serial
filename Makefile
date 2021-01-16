TOPTARGETS := all clean

SUBDIRS := firmware

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)

apple1serial.bin:
	xa -W -C -v -O ASCII -S -c src/apple1serial.xa -l apple1serial.label -o apple1serial.bin

all: apple1serial.bin

clean:
	rm apple1serial.bin apple1serial.label
