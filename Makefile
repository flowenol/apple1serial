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
