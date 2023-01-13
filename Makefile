TOPTARGETS := all clean

SUBDIRS := firmware

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)

apple1serial-C000.bin:
	xa -DOFFSET="0" -W -C -v -O ASCII -c src/apple1serial.a65 -l apple1serial-C000.label -o apple1serial-C000.bin

apple1serial-C300.bin:
	xa -DOFFSET="768" -W -C -v -O ASCII -c src/apple1serial.a65 -l apple1serial-C300.label -o apple1serial-C300.bin


all: apple1serial-C000.bin apple1serial-C300.bin

clean:
	rm -f apple1serial*.bin apple1serial*.label

burn-C000-gal:
	minipro -p GAL20V8B -w mapping/address_decoder-C000.jed

burn-C000-rom: apple1serial-C000.bin
	minipro -s -p 28C64B -w apple1serial-C000.bin

burn-C300-gal:
	minipro -p GAL20V8B -w mapping/address_decoder-C300.jed

burn-C300-rom: apple1serial-C300.bin
	minipro -s -p 28C64B -w apple1serial-C300.bin
