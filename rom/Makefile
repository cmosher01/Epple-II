TARGETS=epple2sys.a65 stdin.a65 stdout.a65 clock.a65

ZEROSEGS=-bd 0 -bb 0 -bz 0

M4=m4
XA65=xa
LDO65=ldo65
RELOC65=reloc65

.SUFFIXES:
.SUFFIXES: .m4 .s65 .o65 .r65 .a65

.m4.s65:
	$(M4) -I ./ $(M4FLAGS) $< >$@

.s65.o65:
	$(XA65) -C -M -R -c -bt 0 $(ZERO_SEGS) -o $@ $(XA65FLAGS) $<

.o65.r65:
	$(LDO65) -bt $(CODE_SEG) $(ZERO_SEGS) -o $@ $(LD65FLAGS) $^

.r65.a65:
	$(RELOC65) -xt -o $@ $(RELOC65FLAGS) $<

.PHONY: all
all: $(TARGETS)
epple2sys.r65: CODE_SEG=64512
epple2sys.s65: epple2sys.m4 asm.m4h
clock.r65: CODE_SEG=0
stdin.r65: CODE_SEG=0
stdout.r65: CODE_SEG=0
